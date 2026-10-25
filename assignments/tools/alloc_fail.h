/* CORRECTED: Test-only allocation fault injection; not a course API. */
#ifndef ALLOC_FAIL_INCLUDED
#define ALLOC_FAIL_INCLUDED
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
void testSetFailure(long countdown);
long testActiveAllocations(void);
void *testMalloc(size_t size);
void *testCalloc(size_t count, size_t size);
void *testRealloc(void *pointer, size_t size);
char *testStrdup(const char *text);
void testFree(void *pointer);
#ifdef EE209_WRAP_ALLOC
#define malloc testMalloc
#define calloc testCalloc
#define realloc testRealloc
#define strdup testStrdup
#define free testFree
#endif
#endif
