/*
Assignment 3
Jeon Sangeon
customer_manager1.c
*/ 
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
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
  /* CORRECTED: 중복 검사 후 문자열/배열 할당을 성공시켜야
     DB 상태를 바꾼다. 실패하면 이전 데이터와 소유권을 유지한다. */
  int i;
  int emptyIndex=-1;
  int idh, nameh;
  char *bufid, *bufname;
  if(!d || !id || !name || purchase<=0) return -1;
  idh=hash_function(id);
  nameh=hash_function(name);

  /* CORRECTED: Find duplicates before allocating or changing the database. */
  for(i=0;i<d->curArrSize;i++) {
    if(!d->pArray[i].name) {
      if(emptyIndex<0) emptyIndex=i;
      continue;
    }
    if(idh==d->pArray[i].idh && !strcmp(d->pArray[i].id,id))
      return -1;
    if(nameh==d->pArray[i].nameh &&
       !strcmp(d->pArray[i].name,name)) return -1;
  }
  bufid=strdup(id);
  bufname=strdup(name);
  if(!bufid || !bufname) {
    free(bufid);
    free(bufname);
    return -1;
  }
  if(emptyIndex<0) {
    int newSize;
    struct UserInfo *newArray;
    if(d->curArrSize>INT_MAX-UNIT_ARRAY_SIZE) {
      free(bufid); free(bufname); return -1;
    }
    newSize=d->curArrSize+UNIT_ARRAY_SIZE;
    if((size_t)newSize>SIZE_MAX/sizeof(*newArray)) {
      free(bufid); free(bufname); return -1;
    }
    newArray=realloc(d->pArray,(size_t)newSize*sizeof(*newArray));
    if(!newArray) {
      free(bufid); free(bufname); return -1;
    }
    /* CORRECTED: realloc does not zero the newly added slots. */
    memset(newArray+d->curArrSize,0,
           UNIT_ARRAY_SIZE*sizeof(*newArray));
    emptyIndex=d->curArrSize;
    d->pArray=newArray;
    d->curArrSize=newSize;
  }
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
		d->pArray[i].name=NULL;
                /* CORRECTED: 해제한 id도 지우고 개수를 줄인다. */
		d->pArray[i].id=NULL;
		d->numItems--;
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
		d->pArray[i].name=NULL;
                /* CORRECTED: 해제한 id도 지우고 개수를 줄인다. */
		d->pArray[i].id=NULL;
		d->numItems--;
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
