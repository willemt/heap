/*
 
Copyright (c) 2011, Willem-Hendrik Thiart
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * The names of its contributors may not be used to endorse or promote
      products derived from this software without specific prior written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL WILLEM-HENDRIK THIART BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <assert.h>

#include "heap.h"

#define DEBUG 0
#define INITIAL_CAPACITY 13

static int __child_left(const int idx)
{
    return idx * 2 + 1;
}

static int __child_right(const int idx)
{
    return idx * 2 + 2;
}

static int __parent(const int idx)
{
    assert(idx != 0);
    return (idx - 1) / 2;
}

heap_t *heap_new(int (*cmp) (const void *,
			     const void *,
			     const void *udata), const void *udata)
{
    heap_t *h;

    h = malloc(sizeof(heap_t));
    h->cmp = cmp;
    h->udata = udata;
    h->arraySize = INITIAL_CAPACITY;
    h->array = malloc(sizeof(void *) * h->arraySize);
    h->count = 0;
    return h;
}

void heap_free(heap_t * h)
{
    free(h->array);
    free(h);
}

static void __ensurecapacity(heap_t * h)
{
    int ii;

    void **array_n;

    if (h->count < h->arraySize)
	return;

    /* double capacity */
    h->arraySize *= 2;
    array_n = malloc(sizeof(void *) * h->arraySize);

    /* copy old data across to new array */
    for (ii = 0; ii < h->count; ii++)
    {
	array_n[ii] = h->array[ii];
	assert(array_n[ii]);
    }

    /* swap arrays */
    free(h->array);
    h->array = array_n;
}

static void __swap(heap_t * h, const int i1, const int i2)
{
    void *tmp;
    
    tmp = h->array[i1];
    h->array[i1] = h->array[i2];
    h->array[i2] = tmp;
}

static int __pushup(heap_t * h, int idx)
{
    while (1)
    {
	int parent, compare;

	/* we are now the root node */
	if (0 == idx)
	    return idx;

	parent = __parent(idx);
	compare = h->cmp(h->array[idx], h->array[parent], h->udata);

	/* we are smaller than the parent */
	if (compare < 0)
	{
	    return -1;
	}
	else
	{
	    __swap(h, idx, parent);
	}

	idx = parent;
    }

    return idx;
}

static void __pushdown(heap_t * h, int idx)
{
    while (1)
    {
	int childl, childr, child, compare;

	assert(idx != h->count);

	childl = __child_left(idx);
	childr = __child_right(idx);
	child = -1;

	if (childr >= h->count)
	{
	    /* can't pushdown any further */
	    if (childl >= h->count)
		return;

	    child = childl;
	}
	else
	{
	    /* find biggest child */
	    compare =
		h->cmp(h->array[childl], h->array[childr], h->udata);

	    if (compare < 0)
	    {
		child = childr;
	    }
	    else
	    {
		child = childl;
	    }
	}

	assert(child != -1);

	compare = h->cmp(h->array[idx], h->array[child], h->udata);

	/* idx is smaller than child */
	if (compare < 0)
	{
	    assert(h->array[idx]);
	    assert(h->array[child]);
	    __swap(h, idx, child);
	    idx = child;
	    /* bigger than the biggest child, we stop, we win */
	}
	else
	{
	    return;
	}
    }
}

void heap_offer(heap_t * h, void *item)
{
    assert(h);
    assert(item);
    if (!item)
	return;

    __ensurecapacity(h);

    h->array[h->count] = item;

    /* ensure heap properties */
    __pushup(h, h->count);

    h->count++;
}

#if DEBUG
static void DEBUG_check_validity(heap_t * h)
{
    int ii;

    for (ii = 0; ii < h->count; ii++)
	assert(h->array[ii]);
}
#endif

void *heap_poll(heap_t * h)
{
    void *item;

    assert(h);

    if (!h)
	return NULL;

    if (0 == heap_count(h))
	return NULL;

#if DEBUG
    DEBUG_check_validity(h);
#endif

    item = h->array[0];

    h->array[0] = NULL;
    __swap(h, 0, h->count - 1);
    h->count--;

#if DEBUG
    DEBUG_check_validity(h);
#endif

    if (h->count > 0)
    {
	assert(h->array[0]);
	__pushdown(h, 0);
    }

#if DEBUG
    DEBUG_check_validity(h);
#endif

    return item;
}

void *heap_peek(heap_t * h)
{
    if (!h)
	return NULL;

    if (0 == heap_count(h))
	return NULL;

    return h->array[0];
}

void heap_clear(heap_t * h)
{
    h->count = 0;
}

/**
 * @return item's index on the heap's array */
static int __item_get_idx(heap_t * h, const void *item)
{
    int compare, idx;

    for (idx = 0; idx < h->count; idx++)
    {
	compare = h->cmp(h->array[idx], item, h->udata);

	/* we have found it */
	if (compare == 0)
	{
	    return idx;
	}
    }

    return -1;
}

void *heap_remove_item(heap_t * h, const void *item)
{
    void *ret_item;
    int idx;

    /* find the index */
    idx = __item_get_idx(h, item);

    /* we didn't find it */
    if (idx == -1)
	return NULL;

    /* swap the item we found with the last item on the heap */
    ret_item = h->array[idx];
    h->array[idx] = h->array[h->count - 1];
    h->array[h->count - 1] = NULL;

    /* decrement counter */
    h->count -= 1;

    /* ensure heap property */
    __pushup(h, idx);

    return ret_item;
}

int heap_contains_item(heap_t * h, const void *item)
{
    int idx;

    /* find the index */
    idx = __item_get_idx(h, item);

    return (idx != -1);
}

int heap_count(heap_t * h)
{
    return h->count;
}

/*--------------------------------------------------------------79-characters-*/
