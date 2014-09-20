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

void TestHeap_new_results_in_empty_heap(
    CuTest * tc
    )
{
    heap_t *hp = heap_new(NULL, NULL);

    CuAssertTrue(tc, 0 == heap_count(hp));
}

void TestHeap_offer_will_not_accept_null_item(
    CuTest * tc
    )
{
    heap_t *hp = heap_new(__uint_compare, NULL);

    CuAssertTrue(tc, -1 == heap_offer(hp, NULL));
    CuAssertTrue(tc, 0 == heap_count(hp));
}

void TestHeap_offerx_will_not_accept_null_item(
    CuTest * tc
    )
{
    heap_t *hp = heap_new(__uint_compare, NULL);

    CuAssertTrue(tc, -1 == heap_offerx(hp, NULL));
    CuAssertTrue(tc, 0 == heap_count(hp));
}

void TestHeap_offer_adds_new_item(
    CuTest * tc
    )
{
    int val = 10;

    heap_t *hp = heap_new(__uint_compare, NULL);

    heap_offer(hp, &val);
    CuAssertTrue(tc, 1 == heap_count(hp));
}

void TestHeap_poll_on_empty_heap_returns_null(
    CuTest * tc
    )
{
    heap_t *hp = heap_new(__uint_compare, NULL);

    CuAssertTrue(tc, NULL == heap_poll(hp));
}

void TestHeap_poll_removes_item(
    CuTest * tc
    )
{
    int val = 10;

    int *res;

    heap_t *hp = heap_new(__uint_compare, NULL);

    heap_offer(hp, &val);
    res = heap_poll(hp);

    CuAssertTrue(tc, 0 == heap_count(hp));
    CuAssertTrue(tc, res == &val);
}

void TestHeap_poll_removes_best_item(
    CuTest * tc
    )
{
    int vals[10] = { 9, 2, 5, 7, 4, 6, 3, 8, 1 };
    int ii;

    heap_t *hp = heap_new(__uint_compare, NULL);

    for (ii = 0; ii < 9; ii++)
        heap_offer(hp, &vals[ii]);
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

void TestHeap_remove_item_using_cmp_callback_works(
    CuTest * tc
    )
{
    heap_t *hp = heap_new(__uint_compare, NULL);

    int vals[10] = { 9, 2, 5, 7, 4, 6, 3, 8, 1 };
    int ii;

    for (ii = 0; ii < 9; ii++)
        heap_offer(hp, &vals[ii]);

    int *item = heap_remove_item(hp, &vals[0]);

    CuAssertTrue(tc, *item == 9);

    heap_free(hp);
}

void TestHeap_remove_item_using_cmp_callback_returns_null_for_missing_item(
    CuTest * tc
    )
{
    heap_t *hp = heap_new(__uint_compare, NULL);

    int vals[10] = { 9, 2, 5, 7, 4, 6, 3, 8, 1 };
    int ii;

    for (ii = 1; ii < 9; ii++)
        heap_offer(hp, &vals[ii]);

    CuAssertTrue(tc, NULL == heap_remove_item(hp, &vals[0]));

    heap_free(hp);
}

void TestHeap_contains_item_using_cmp_callback_works(
    CuTest * tc
    )
{
    int vals[10] = { 9, 2, 5, 7, 4, 6, 3, 8, 1 };
    int ii;

    heap_t *hp = heap_new(__uint_compare, NULL);

    for (ii = 0; ii < 9; ii++)
        heap_offer(hp, &vals[ii]);

    CuAssertTrue(tc, 1 == heap_contains_item(hp, &vals[2]));
    CuAssertTrue(tc, 1 == heap_contains_item(hp, &vals[1]));
    CuAssertTrue(tc, 1 == heap_contains_item(hp, &vals[0]));

    heap_free(hp);
}

void TestHeap_clear_removes_all_items(
    CuTest * tc
    )
{
    int vals[10] = { 9, 2, 5, 7, 4, 6, 3, 8, 1 };
    int ii;

    heap_t *hp = heap_new(__uint_compare, NULL);

    for (ii = 0; ii < 9; ii++)
        heap_offer(hp, &vals[ii]);

    heap_clear(hp);

    CuAssertTrue(tc, 0 == heap_count(hp));
    CuAssertTrue(tc, 0 == heap_contains_item(hp, &vals[2]));
    CuAssertTrue(tc, 0 == heap_contains_item(hp, &vals[1]));
    CuAssertTrue(tc, 0 == heap_contains_item(hp, &vals[0]));

    heap_free(hp);
}

void TestHeap_peek_returns_null_if_heap_is_empty(
    CuTest * tc
    )
{
    heap_t *hp = heap_new(__uint_compare, NULL);

    CuAssertTrue(tc, NULL == heap_peek(hp));

    heap_free(hp);
}

void TestHeap_peek_gets_best_item(
    CuTest * tc
    )
{
    int ii;
    int vals[10] = { 9, 2, 5, 7, 4, 6, 3, 8, 1 };

    heap_t *hp = heap_new(__uint_compare, NULL);

    for (ii = 0; ii < 9; ii++)
        heap_offer(hp, &vals[ii]);

    CuAssertTrue(tc, 1 == *(int*)heap_peek(hp));

    heap_free(hp);
}

void TestHeap_offer_ensures_capacity_is_sufficient(
    CuTest * tc
    )
{
    heap_t *hp;
    void** array;
    int vals[3] = { 1, 2, 3 };

    hp = malloc(sizeof(heap_t));
    array = malloc(sizeof(void *) * 1);
    heap_init(hp, __uint_compare, NULL, array, 1);

    CuAssertTrue(tc, 0 == heap_offer(hp, &vals[0]));
    CuAssertTrue(tc, 1 == heap_size(hp));
    CuAssertTrue(tc, 0 == heap_offer(hp, &vals[1]));
    CuAssertTrue(tc, 2 == heap_size(hp));
    CuAssertTrue(tc, 0 == heap_offer(hp, &vals[2]));
    CuAssertTrue(tc, 4 == heap_size(hp));
    CuAssertTrue(tc, 3 == heap_count(hp));
    heap_free(hp);
}

void TestHeap_doubling_capacity_retains_items(
    CuTest * tc
    )
{
    int vals[15] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    int ii;

    heap_t *hp = heap_new(__uint_compare, NULL);

    for (ii = 0; ii < 15; ii++)
        heap_offer(hp, &vals[ii]);

    CuAssertTrue(tc, 1 == heap_contains_item(hp, &vals[0]));

    heap_free(hp);
}

void TestHeap_offerx_fails_if_not_enough_capacity(
    CuTest * tc
    )
{
    heap_t *hp;
    void** array;
    int vals[3] = { 1, 2, 3 };

    hp = malloc(sizeof(heap_t));
    array = malloc(sizeof(void *) * 2);
    heap_init(hp, __uint_compare, NULL, array, 2);

    CuAssertTrue(tc, 0 == heap_offerx(hp, &vals[0]));
    CuAssertTrue(tc, 0 == heap_offerx(hp, &vals[1]));
    CuAssertTrue(tc, -1 == heap_offerx(hp, &vals[2]));
    CuAssertTrue(tc, 2 == heap_count(hp));
    CuAssertTrue(tc, 0 == heap_contains_item(hp, &vals[2]));
    heap_free(hp);
}
