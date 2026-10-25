/* CORRECTED: Assignment 3, Jeon Sangeon: interface reconstructed from the PDF.
   All operations use only d; none perform normal output. */
#ifndef CUSTOMER_MANAGER_INCLUDED
#define CUSTOMER_MANAGER_INCLUDED

/* CORRECTED: An independently owned customer collection with unique IDs/names. */
typedef struct DB *DB_T;
/* CORRECTED: A callback computes a nonnegative contribution for one customer.
   It must not mutate/destroy the database during iteration. */
typedef int (*FUNCPTR_T)(const char *id, const char *name,
                         const int purchase);

/* CORRECTED: Return a new empty collection or NULL on allocation failure. */
DB_T CreateCustomerDB(void);
/* CORRECTED: Release d and all owned data. NULL is accepted. */
void DestroyCustomerDB(DB_T d);
/* CORRECTED: Copy id/name and register purchase (>0); return 0, or -1 on invalid
   parameters, duplicate ID/name or allocation failure. */
int RegisterCustomer(DB_T d, const char *id, const char *name,
                     const int purchase);
/* CORRECTED: Remove and free the matching record; return 0 or -1 if absent or
   either argument is NULL. */
int UnregisterCustomerByID(DB_T d, const char *id);
int UnregisterCustomerByName(DB_T d, const char *name);
/* CORRECTED: Return the matching purchase, or -1 for missing/NULL arguments. */
int GetPurchaseByID(DB_T d, const char *id);
int GetPurchaseByName(DB_T d, const char *name);
/* CORRECTED: Apply fp once per record and return its sum, or -1 for NULL d/fp.
   The caller ensures that the sum is representable as an int. */
int GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp);
#endif
