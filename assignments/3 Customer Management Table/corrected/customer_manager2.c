/*
Assignment 3
Jeon Sangeon
customer_manager2.c
*/ 
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define UNIT_ARRAY_SIZE 1024
#define HASH_MULTIPLIER 65599


struct UserInfo {
  // A struct for a user infomation
  char *name;				// customer name
  char *id;				// customer id
  int purchase;				// purchase amount (> 0)

  struct UserInfo* pastUbyname;		// pointer to past UserInfo in terms of name
  struct UserInfo* pastUbyid;		// pointer to past UserInfo in terms of id
  struct UserInfo* nextUbyname;		// pointer to next UserInfo in terms of name
  struct UserInfo* nextUbyid;		// pointer to next UserInfo in terms of id
};

struct DB {
  //A struct for the hashtable consisting of UserInfo
  struct UserInfo** namePtArr;		// pointer to the hashtable for the name
  struct UserInfo** idPtArr;		// pointer to the hashtable for the id
  int arrLen;				// length of hashtable
  int nodeNum;				// the number of node
};


/*--------------------------------------------------------------------*/




/*--------------------------------------------------------------------*/
// Basic functions
/*--------------------------------------------------------------------*/

static int hash_function(const char *pcKey, int iBucketCount)

/* Return a hash code for pcKey that is between 0 and iBucketCount-1,
   inclusive. Adapted from the EE209 lecture notes. */
{
   int i;
   unsigned int uiHash = 0U;
   for (i = 0; pcKey[i] != '\0'; i++)
      uiHash = uiHash * (unsigned int)HASH_MULTIPLIER
               + (unsigned int)pcKey[i];
   return (int)(uiHash & (unsigned int)(iBucketCount-1));
}
//finish
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
  /* CORRECTED: 두 배열 모두 성공한 경우에만 DB를 반환한다. */
  d->arrLen=UNIT_ARRAY_SIZE;
  d->namePtArr=calloc((size_t)d->arrLen,sizeof(*d->namePtArr));
  d->idPtArr=calloc((size_t)d->arrLen,sizeof(*d->idPtArr));
  if(!d->namePtArr || !d->idPtArr) {
    free(d->namePtArr);
    free(d->idPtArr);
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
  if(d==NULL) return;
  struct UserInfo* u;
  struct UserInfo* nextu;
  int i;
  for(i=0;i<d->arrLen;i++)
	for(u=d->idPtArr[i]; u!=NULL;u=nextu){
		nextu=u->nextUbyid;
                /* CORRECTED: 각 노드의 두 문자열도 소유한다. */
		free(u->id);
		free(u->name);
		free(u);
	}
  /* CORRECTED: 인덱스 배열까지 해제한다. */
  free(d->idPtArr);
  free(d->namePtArr);
  free(d);
}

/*--------------------------------------------------------------------*/

static struct UserInfo*
SearchByID(DB_T d, const char* id, int* desth)
/* Return the pointer of the UserInfo whose id is the parameter id
   If not found, return NULL
   restore hash code of id in desth*/
{
  struct UserInfo* u;
  /* CORRECTED: 모든 노드는 현재 크기로 재해시되어 있다. */
  int h=hash_function(id,d->arrLen);
  *desth=h;
  for(u=d->idPtArr[h]; u; u=u->nextUbyid)
    if(!strcmp(u->id,id)) return u;
  return NULL;
}

/*--------------------------------------------------------------------*/

static struct UserInfo*
SearchByName(DB_T d, const char* name, int* desth)
/* Return the pointer of the UserInfo whose name is the parameter name
   If not found, return NULL
   restore hash code of name in desth*/
{
  struct UserInfo* u;
  /* CORRECTED: 현재 크기의 버킷 하나에서만 찾는다. */
  int h=hash_function(name,d->arrLen);
  *desth=h;
  for(u=d->namePtArr[h]; u; u=u->nextUbyname)
    if(!strcmp(u->name,name)) return u;
  return NULL;
}

/*--------------------------------------------------------------------*/

static int UnregisterUser(DB_T d, struct UserInfo* u, int idh, int nameh)
/*Free u and link past and next node of u in all hashtable*/
{
  struct UserInfo* pastidu=u->pastUbyid;
  struct UserInfo* pastnameu=u->pastUbyname;
  struct UserInfo* nextidu=u->nextUbyid;
  struct UserInfo* nextnameu=u->nextUbyname;

  if(pastidu && nextidu){
	pastidu->nextUbyid=nextidu;
	nextidu->pastUbyid=pastidu;
  }else if(pastidu && !nextidu)
	pastidu->nextUbyid=NULL;
  else if(!pastidu && nextidu){
	d->idPtArr[idh]=nextidu;
	nextidu->pastUbyid=NULL;
  }else if(!pastidu && !nextidu)
	d->idPtArr[idh]=NULL;

  if(pastnameu && nextnameu){
	pastnameu->nextUbyname=nextnameu;
	nextnameu->pastUbyname=pastnameu;
  }else if(pastnameu && !nextnameu)
	pastnameu->nextUbyname=NULL;
  else if(!pastnameu && nextnameu){
	d->namePtArr[nameh]=nextnameu;
	nextnameu->pastUbyname=NULL;
  }else if(!pastnameu && !nextnameu)
	d->namePtArr[nameh]=NULL;
  free(u->name), free(u->id);
  free(u);
  /* CORRECTED: 실제 노드 수와 확장 임계값을 일치시킨다. */
  d->nodeNum--;
  return 0;
}
/*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*/
// Advanced functions
/*--------------------------------------------------------------------*/

/* CORRECTED: Double both indexes and rehash every node. On allocation failure,
   leave d untouched and return 0. No I/O or global state is used. */
static int ExpandCustomerDB(DB_T d)
{
  int i, idh, nameh;
  int newLen=d->arrLen*2;
  struct UserInfo **newIDpt, **newNamept;
  struct UserInfo *u, *next;
  assert(d!=NULL);
  newIDpt=calloc((size_t)newLen,sizeof(*newIDpt));
  newNamept=calloc((size_t)newLen,sizeof(*newNamept));
  if(!newIDpt || !newNamept) {
    free(newIDpt); free(newNamept); return 0;
  }
  for(i=0;i<d->arrLen;i++) {
    for(u=d->idPtArr[i];u;u=next) {
      next=u->nextUbyid;
      idh=hash_function(u->id,newLen);
      nameh=hash_function(u->name,newLen);
      u->pastUbyid=NULL;
      u->nextUbyid=newIDpt[idh];
      if(newIDpt[idh]) newIDpt[idh]->pastUbyid=u;
      newIDpt[idh]=u;
      u->pastUbyname=NULL;
      u->nextUbyname=newNamept[nameh];
      if(newNamept[nameh]) newNamept[nameh]->pastUbyname=u;
      newNamept[nameh]=u;
    }
  }
  free(d->idPtArr); free(d->namePtArr);
  d->idPtArr=newIDpt; d->namePtArr=newNamept;
  d->arrLen=newLen;
  return 1;
}

int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
/* Register the customer's infomation in d
   if success, return 0, otherwise return -1 */
{
  int idh, nameh;
  struct UserInfo *u;
  if(!d || !id || !name || purchase<=0) return -1;
  if(SearchByID(d,id,&idh)) return -1;
  if(SearchByName(d,name,&nameh)) return -1;
  /* CORRECTED: calloc initializes both previous pointers of a new head. */
  u=calloc(1,sizeof(*u));
  if(!u) return -1;
  u->name=strdup(name);
  u->id=strdup(id);
  if(!u->name || !u->id) {
    free(u->name); free(u->id); free(u); return -1;
  }
  if(d->arrLen<(1<<20) && d->nodeNum+1>=d->arrLen*3/4) {
    if(!ExpandCustomerDB(d)) {
      free(u->name); free(u->id); free(u); return -1;
    }
    idh=hash_function(id,d->arrLen);
    nameh=hash_function(name,d->arrLen);
  }
  u->purchase=purchase;
  if(d->namePtArr[nameh]) d->namePtArr[nameh]->pastUbyname=u;
  u->nextUbyname=d->namePtArr[nameh];
  d->namePtArr[nameh]=u;
  if(d->idPtArr[idh]) d->idPtArr[idh]->pastUbyid=u;
  u->nextUbyid=d->idPtArr[idh];
  d->idPtArr[idh]=u;
  d->nodeNum++;
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
  int idh;
  struct UserInfo* u=NULL;
  if(!(u=SearchByID(d,id,&idh))) return -1;
  int nameh=hash_function(u->name, d->arrLen);
  UnregisterUser(d,u,idh,nameh);
  return 0;
}

/*--------------------------------------------------------------------*/
int
UnregisterCustomerByName(DB_T d, const char *name)
/* Unregister the UserInfo whose name is same with parameter name
   Failure when there's no such customer
   if success, return 0, otherwise return -1*/
{
  if(!d || !name) return -1;
  int nameh;
  struct UserInfo* u=NULL;
  if(!(u=SearchByName(d,name,&nameh))) return -1;
  int idh=hash_function(u->id, d->arrLen);
  UnregisterUser(d,u,idh,nameh);
  return 0;
}

/*--------------------------------------------------------------------*/
int
GetPurchaseByID(DB_T d, const char* id)
/* Find the user whose id is same with the parameter id and
   Return  its purchase
   Failure when there's no such customer and return -1 */
{
  if(!d || !id) return -1;
  int idh;
  struct UserInfo* u=SearchByID(d,id,&idh);
  return u ? u->purchase :-1;
}

/*--------------------------------------------------------------------*/
int
GetPurchaseByName(DB_T d, const char* name)
/* Find the user whose name is same with the parameter name and
   Return  its purchase
   Failure when there's no such customer and return -1 */
{
  if(!d || !name) return -1;
  int nameh;
  struct UserInfo* u=SearchByName(d,name,&nameh);
  return u ? u->purchase :-1;
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
  struct UserInfo* u;
  for(i=0;i<d->arrLen;i++)
	for(u=d->idPtArr[i]; u!=NULL;u=u->nextUbyid){
		sum+=fp(u->id,u->name,u->purchase);
	}
  return sum;
}
