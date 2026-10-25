/* CORRECTED: Assignment 3, Jeon Sangeon. Added regression/benchmark
   client; the archive did not include the course testclient/header. */
#include "customer_manager.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef TEST_ALLOC_FAULTS
#include "alloc_fail.h"
#endif
static int calls;
/* CORRECTED: Count visits and return purchase for summation. */
static int contribution(const char *id, const char *name, int purchase)
{
    assert(id != NULL && name != NULL);
    calls++;
    return purchase;
}
/* CORRECTED: Generate distinct, deterministic ID/name pairs. */
static void keys(int index, char *id, char *name)
{
    assert(id != NULL && name != NULL);
    sprintf(id, "id-%08d", index);
    sprintf(name, "name-%08d", index);
}
#ifdef TEST_ALLOC_FAULTS
/* CORRECTED: Fail each allocation site in create, insert and growth.
   A failure must preserve existing users and leak no allocations. */
static void faultCases(void)
{
    int budget, count, i;
    for (budget = 0; budget < 4; budget++) {
        DB_T db;
        testSetFailure(budget);
        db = CreateCustomerDB();
        testSetFailure(-1);
        DestroyCustomerDB(db);
        assert(testActiveAllocations() == 0);
    }
    for (count = 0; count <= 1024; count += count == 0 ? 767 : 257) {
        for (budget = 0; budget < 6; budget++) {
            DB_T db = CreateCustomerDB();
            char id[32], name[32];
            long before;
            int result;
            assert(db != NULL);
            for (i = 0; i < count; i++) {
                keys(i, id, name);
                assert(RegisterCustomer(db, id, name, i + 1) == 0);
            }
            before = testActiveAllocations();
            testSetFailure(budget);
            result = RegisterCustomer(db, "fresh", "Fresh Name", 7);
            testSetFailure(-1);
            if (result == -1) {
                assert(GetPurchaseByID(db, "fresh") == -1);
                assert(GetPurchaseByName(db, "Fresh Name") == -1);
                assert(testActiveAllocations() == before);
            }
            for (i = 0; i < count; i++) {
                keys(i, id, name);
                assert(GetPurchaseByID(db, id) == i + 1);
                assert(GetPurchaseByName(db, name) == i + 1);
            }
            DestroyCustomerDB(db);
            assert(testActiveAllocations() == 0);
        }
    }
}
#endif
/* CORRECTED: Exercise both implementations through the same public
   API, including growth, deletion, reuse, independent DBs and ownership. */
int main(void)
{
    enum {COUNT = 4096};
    DB_T db, other;
    char id[32], name[32];
    int i;
    clock_t started = clock();
    DestroyCustomerDB(NULL);
    assert(RegisterCustomer(NULL, "id", "name", 1) == -1);
    db = CreateCustomerDB(); other = CreateCustomerDB();
    assert(db != NULL && other != NULL);
    assert(RegisterCustomer(db, NULL, "name", 1) == -1);
    assert(RegisterCustomer(db, "id", NULL, 1) == -1);
    assert(RegisterCustomer(db, "id", "name", 0) == -1);
    assert(RegisterCustomer(db, "id", "name", -1) == -1);
    assert(GetPurchaseByID(NULL, "id") == -1);
    assert(GetPurchaseByName(db, NULL) == -1);
    assert(UnregisterCustomerByID(NULL, "id") == -1);
    assert(UnregisterCustomerByName(db, NULL) == -1);
    assert(GetSumCustomerPurchase(db, NULL) == -1);
    assert(GetSumCustomerPurchase(NULL, contribution) == -1);
    assert(GetSumCustomerPurchase(db, contribution) == 0);
    for (i = 0; i < COUNT; i++) {
        keys(i, id, name);
        assert(RegisterCustomer(db, id, name, i + 1) == 0);
        assert(RegisterCustomer(other, id, name, 1) == 0);
    }
    /* CORRECTED: Input arrays were overwritten on every iteration;
       successful lookups prove that the DB owns copies. */
    for (i = 0; i < COUNT; i++) {
        keys(i, id, name);
        assert(GetPurchaseByID(db, id) == i + 1);
        assert(GetPurchaseByName(db, name) == i + 1);
        assert(RegisterCustomer(db, id, "new-name", 7) == -1);
        assert(RegisterCustomer(db, "new-id", name, 7) == -1);
    }
    calls = 0;
    assert(GetSumCustomerPurchase(db, contribution) == COUNT*(COUNT+1)/2);
    assert(calls == COUNT);
    for (i = 0; i < COUNT; i++) {
        keys(i, id, name);
        assert((i % 2 ? UnregisterCustomerByName(db, name)
                      : UnregisterCustomerByID(db, id)) == 0);
        assert(GetPurchaseByID(db, id) == -1);
        assert(GetPurchaseByName(db, name) == -1);
        assert(UnregisterCustomerByID(db, id) == -1);
    }
    for (i = 0; i < 2048; i++) {
        assert(RegisterCustomer(db, "reused", "Reused", 1) == 0);
        assert(UnregisterCustomerByName(db, "Reused") == 0);
    }
    assert(GetPurchaseByID(other, "id-00000000") == 1);
    DestroyCustomerDB(db); DestroyCustomerDB(other);
#ifdef TEST_ALLOC_FAULTS
    assert(testActiveAllocations() == 0);
    faultCases();
    puts("Customer DB: allocation failure/rollback/leak checks passed");
#endif
    printf("Customer DB: 4096 records, growth/delete/reuse/two DBs passed (%.3fs)\n",
           (double)(clock() - started) / CLOCKS_PER_SEC);
    return 0;
}
