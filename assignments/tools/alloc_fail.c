/* CORRECTED: Test-only allocator wrappers. Track live allocations and
   inject deterministic failures without changing production sources. */
#include "alloc_fail.h"
#include <assert.h>
#include <errno.h>
static long remaining = -1;
static long active;
void testSetFailure(long countdown) { remaining = countdown; }
long testActiveAllocations(void) { return active; }
static int fails(void)
{
    if (remaining < 0) return 0;
    if (remaining-- == 0) { errno = ENOMEM; return 1; }
    return 0;
}
void *testMalloc(size_t size)
{
    void *p;
    if (fails()) return NULL;
    p = malloc(size);
    if (p) active++;
    return p;
}
void *testCalloc(size_t count, size_t size)
{
    void *p;
    if (fails()) return NULL;
    p = calloc(count, size);
    if (p) active++;
    return p;
}
void *testRealloc(void *pointer, size_t size)
{
    void *p;
    if (fails()) return NULL;
    p = realloc(pointer, size);
    if (p && !pointer) active++;
    return p;
}
char *testStrdup(const char *text)
{
    char *p;
    assert(text != NULL);
    p = testMalloc(strlen(text) + 1);
    if (p) strcpy(p, text);
    return p;
}
void testFree(void *pointer)
{
    if (pointer) { assert(active > 0); active--; }
    free(pointer);
}
