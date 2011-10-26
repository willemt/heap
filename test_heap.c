#include <stdbool.h>
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"

#include "heap.h"

static unsigned int __uint_hash(
    const void *e1
)
{
    const long i1 = (unsigned long) e1;

    assert(i1 >= 0);
    return i1;
}

static int __uint_compare(
    const void *e1,
    const void *e2
)
{
    const int *i1 = e1;

    const int *i2 = e2;

    return *i2 - *i1;
}

void TestHeap_new(
    CuTest * tc
)
{
    heap_t *hp;

    hp = heap_new(__uint_compare);

    CuAssertTrue(tc, 0 == heap_count(hp));
}

void TestHeap_offer(
    CuTest * tc
)
{
    heap_t *hp;

    int val = 10;

    hp = heap_new(__uint_compare);

    heap_offer(hp, &val);
    CuAssertTrue(tc, 1 == heap_count(hp));
}

void TestHeap_poll(
    CuTest * tc
)
{
    heap_t *hp;

    int val = 10;

    int *res;

    hp = heap_new(__uint_compare);

    heap_offer(hp, &val);
    res = heap_poll(hp);

    CuAssertTrue(tc, 0 == heap_count(hp));
    CuAssertTrue(tc, res == &val);
}

void TestHeap_poll_best(
    CuTest * tc
)
{
    heap_t *hp;

    int vals[10] = { 9, 2, 5, 7, 4, 6, 3, 8, 1 };
    int ii;

    hp = heap_new(__uint_compare);

    for (ii = 0; ii < 9; ii++)
    {
        heap_offer(hp, &vals[ii]);
    }
    CuAssertTrue(tc, 9 == heap_count(hp));

    for (ii = 0; ii < 9; ii++)
    {
        int *res;

        res = heap_poll(hp);
        CuAssertTrue(tc, *res == ii + 1);
    }
    CuAssertTrue(tc, 0 == heap_count(hp));

    heap_free(hp);
}
