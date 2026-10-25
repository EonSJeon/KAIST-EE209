/*
Assignment 3
Jeon Sangeon
customer_manager1.c
*/ 
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* WRONG: 이 저장본에는 customer_manager.h가 없어 독립 빌드가 불가능하다. corrected에 PDF의 공개 API대로 헤더를 복원했다. */
#include "customer_manager.h"

#define UNIT_ARRAY_SIZE 1024
#define HASH_MULTIPLIER 65599

struct UserInfo {
  // A struct for a user infomation
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)

  // Hash code is needed for the fast comparison(id, name)
  int idh;		     // hash code of id
  int nameh;		     // hash code of name
};

struct DB {
  //A struct for the array consisting of UserInfo
  struct UserInfo *pArray;   // pointer to the array
  int curArrSize;            // current array size (max # of elements)
  int numItems;              // # of stored items, needed to determine
			     // # whether the array should be expanded
			     // # or not
};



/*--------------------------------------------------------------------*/
static int hash_function(const char *pcKey)

/* Return a hash code for pcKey that is between 0 and 2047,
   inclusive. Adapted from the EE209 lecture notes. */
{
   int i;
   unsigned int uiHash = 0U;
   for (i = 0; pcKey[i] != '\0'; i++)
      uiHash = uiHash * (unsigned int)HASH_MULTIPLIER
               + (unsigned int)pcKey[i];
   return (int)(uiHash & 2047);
}
/*--------------------------------------------------------------------*/
DB_T
CreateCustomerDB(void)
/* Create DB structure and return its pointer DB_T d */
{
  DB_T d;
  
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 1024 elements
  d->pArray = (struct UserInfo *)calloc(d->curArrSize,sizeof(struct UserInfo));
  if (d->pArray == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",d->curArrSize);   
    free(d);
    return NULL;
  }
  return d;
}
/*--------------------------------------------------------------------*/
void
DestroyCustomerDB(DB_T d)
/* Free all the infomation in d*/
{
  if(d==NULL)return;
  int i;
  for(i=0;i<d->curArrSize;i++)
	if(d->pArray[i].name)
		free(d->pArray[i].name),free(d->pArray[i].id);
  free(d->pArray);
  free(d);
  return;
}

/*--------------------------------------------------------------------*/
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
/* Register the customer's infomation in d
   if success, return 0, otherwise return -1 */
{
  if(!d || !id || !name) return -1;
  if(purchase<=0) return -1;
/* WRONG: 확장 전에 중복/빈 슬롯을 확인하지 않으며 numItems도 삭제 시 감소하지 않는다. 불필요한 확장과 상태 불일치가 생긴다. */
  if(d->numItems>=d->curArrSize){
	d->curArrSize+=1024;
/* WRONG: realloc 결과를 소유 포인터에 바로 대입하면 실패 시 기존 배열을 잃는다. 확장된 슬롯은 0으로 초기화되지도 않는다. */
	d->pArray=realloc(d->pArray,(d->curArrSize)*sizeof(struct UserInfo));
	if(!d->pArray){
		fprintf(stderr, "Can't allocate a memory for array of size %d\n",d->curArrSize);
		return -1;
	}
  }		//Elongate the array if it lacks of space
  int i;
  char emptyFound=0;
  int emptyIndex=0;
  int idh= hash_function(id);
  int nameh= hash_function(name);

  for(i=0;i<d->curArrSize;i++){
	if(!emptyFound && !d->pArray[i].name) emptyIndex=i, emptyFound=1;
	if(d->pArray[i].id && idh==d->pArray[i].idh && !strcmp(d->pArray[i].id,id)) return -1;
	if(d->pArray[i].name && nameh==d->pArray[i].nameh && !strcmp(d->pArray[i].name,name)) return -1;
  }		//Find the first empty space and if there's same id and name return -1

/* WRONG: 두 strdup의 실패를 검사하지 않는다. 하나만 성공한 경우에도 rollback과 free가 필요하다. */
  char* bufid=strdup(id);
  char* bufname=strdup(name);
  d->pArray[emptyIndex].id=bufid;
  d->pArray[emptyIndex].name=bufname;
  d->pArray[emptyIndex].purchase=purchase;
  d->pArray[emptyIndex].idh=idh;
  d->pArray[emptyIndex].nameh=nameh;
  d->numItems++;
  return 0;
}
/*--------------------------------------------------------------------*/
int
UnregisterCustomerByID(DB_T d, const char *id)
/* Unregister the UserInfo whose id is same with parameter id
   Failure when there's no such customer
   if success, return 0, otherwise return -1*/
{
  if(!d || !id) return -1;
  int i;
  int idh=hash_function(id);
  for(i=0;i<d->curArrSize;i++){
	if(d->pArray[i].name && idh==d->pArray[i].idh 
	  && !strcmp(d->pArray[i].id,id)){
		free(d->pArray[i].name),free(d->pArray[i].id);
/* WRONG: id가 해제된 주소로 남고 numItems가 줄지 않는다. 이후 등록의 중복 검사에서 해제된 id를 strcmp로 읽을 수 있다. */
		d->pArray[i].name=NULL;
		return 0;
	}
  }
  return -1;
}
/*--------------------------------------------------------------------*/
int
UnregisterCustomerByName(DB_T d, const char *name)
/* Unregister the UserInfo whose name is same with parameter name
   Failure when there's no such customer
   if success, return 0, otherwise return -1*/
{
  if(!d || !name) return -1;
  int i;
  int nameh=hash_function(name);
  for(i=0;i<d->curArrSize;i++){
	if(d->pArray[i].name && nameh==d->pArray[i].nameh 
	  && !strcmp(d->pArray[i].name,name)){
		free(d->pArray[i].name),free(d->pArray[i].id);
/* WRONG: id가 해제된 주소로 남고 numItems가 줄지 않는다. 이후 등록의 중복 검사에서 해제된 id를 strcmp로 읽을 수 있다. */
		d->pArray[i].name=NULL;
		return 0;
	}
  }
  return -1;
}

/*--------------------------------------------------------------------*/
int
GetPurchaseByID(DB_T d, const char* id)
/* Find the user whose id is same with the parameter id and
   Return  its purchase
   Failure when there's no such customer and return -1 */
{
  if(!d || !id) return -1;
  int i;
  int idh=hash_function(id);
  for(i=0;i<d->curArrSize;i++)
	if(d->pArray[i].name && idh==d->pArray[i].idh 
	  && !strcmp(d->pArray[i].id,id))
		return d->pArray[i].purchase;
  return -1;
}
/*--------------------------------------------------------------------*/
int
GetPurchaseByName(DB_T d, const char* name)
/* Find the user whose name is same with the parameter name and
   Return  its purchase
   Failure when there's no such customer and return -1 */
{
  if(!d || !name) return -1;
  int i;
  int nameh=hash_function(name);
  for(i=0;i<d->curArrSize;i++)
	if(d->pArray[i].name && nameh==d->pArray[i].nameh 
	  && !strcmp(d->pArray[i].name,name))
		return d->pArray[i].purchase;
  return -1;
}
/*--------------------------------------------------------------------*/
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
/* Calculate the sum of the numbers returned by fp by calling fp for each user item
   Return its result
   fp gets user's id, name and purchase as parameters */
{
  if(!d || !fp) return -1;
  int sum=0;
  int i;
  for(i=0;i<d->curArrSize;i++)
	if(d->pArray[i].name)
		sum+=fp(d->pArray[i].id,d->pArray[i].name,d->pArray[i].purchase);
  return sum;
}
