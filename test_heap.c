#include <stdbool.h>
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"

#include "heap.h"

static int __uint_compare(
    const void *e1,
    const void *e2,
    const void *udata __attribute__((__unused__))
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

    hp = heap_new(NULL, NULL);

    CuAssertTrue(tc, 0 == heap_count(hp));
}

void TestHeap_offer(
    CuTest * tc
)
{
    heap_t *hp;

    int val = 10;

    hp = heap_new(__uint_compare, NULL);

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

    hp = heap_new(__uint_compare, NULL);

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

    hp = heap_new(__uint_compare, NULL);

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

void TestHeap_remove_item(
    CuTest * tc
)
{
    heap_t *hp;

    int vals[10] = { 9, 2, 5, 7, 4, 6, 3, 8, 1 };
    int ii;

    hp = heap_new(__uint_compare, NULL);

    for (ii = 0; ii < 9; ii++)
    {
        heap_offer(hp, &vals[ii]);
    }

    int *item;

    item = heap_remove_item(hp, &vals[0]);

    CuAssertTrue(tc, *item == 9);

    heap_free(hp);
}

void TestHeap_contains_item(
    CuTest * tc
)
{
    heap_t *hp;

    int vals[10] = { 9, 2, 5, 7, 4, 6, 3, 8, 1 };
    int ii;

    hp = heap_new(__uint_compare, NULL);

    for (ii = 0; ii < 9; ii++)
    {
        heap_offer(hp, &vals[ii]);
    }

    CuAssertTrue(tc, 1 == heap_contains_item(hp, &vals[2]));
    CuAssertTrue(tc, 1 == heap_contains_item(hp, &vals[1]));
    CuAssertTrue(tc, 1 == heap_contains_item(hp, &vals[0]));

    heap_free(hp);
}

void TestHeap_clearRemovesAllItems(
    CuTest * tc
)
{
    heap_t *hp;

    int vals[10] = { 9, 2, 5, 7, 4, 6, 3, 8, 1 };
    int ii;

    hp = heap_new(__uint_compare, NULL);

    for (ii = 0; ii < 9; ii++)
    {
        heap_offer(hp, &vals[ii]);
    }

    heap_clear(hp);

    CuAssertTrue(tc, 0 == heap_contains_item(hp, &vals[2]));
    CuAssertTrue(tc, 0 == heap_contains_item(hp, &vals[1]));
    CuAssertTrue(tc, 0 == heap_contains_item(hp, &vals[0]));

    heap_free(hp);
}

void TestHeap_peekGetsBest(
    CuTest * tc
)
{
    heap_t *hp;

    int vals[10] = { 9, 2, 5, 7, 4, 6, 3, 8, 1 };
    int ii;

    hp = heap_new(__uint_compare, NULL);

    for (ii = 0; ii < 9; ii++)
    {
        heap_offer(hp, &vals[ii]);
    }

    CuAssertTrue(tc, 1 == *(int*)heap_peek(hp));

    heap_free(hp);
}

void TestHeap_doubling_capacity_retains_items(
    CuTest * tc
)
{
    heap_t *hp;

    int vals[15] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    int ii;

    hp = heap_new(__uint_compare, NULL);

    for (ii = 0; ii < 15; ii++)
    {
        heap_offer(hp, &vals[ii]);
    }

    CuAssertTrue(tc, 1 == heap_contains_item(hp, &vals[0]));

    heap_free(hp);
}
