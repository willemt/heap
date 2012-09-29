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

/*-------------------------------------------------------------- INIT METHODS */

/**
 * Init a heap and return it.
 * We malloc space for it.
 *
 * @param cmp : a function pointer used to determine the priority of the item */
heap_t *heap_new(
    int (*cmp) (const void *,
                const void *,
                const void *udata),
    const void *udata
)
{
    heap_t *hp;

    hp = malloc(sizeof(heap_t));
    hp->cmp = cmp;
    hp->udata = udata;
    hp->arraySize = INITIAL_CAPACITY;
    hp->array = malloc(sizeof(void *) * hp->arraySize);
    hp->count = 0;
    return hp;
}

void heap_free(
    heap_t * hp
)
{
    free(hp->array);
    free(hp);
}

static void __ensurecapacity(
    heap_t * hp
)
{
    int ii;

    void **array_n;

    if (hp->count < hp->arraySize)
        return;

    hp->arraySize *= 2;

    array_n = malloc(sizeof(void *) * hp->arraySize);

    for (ii = 0; ii < hp->count; ii++)
    {
        array_n[ii] = hp->array[ii];
        assert(array_n[ii]);
    }

    free(hp->array);
    hp->array = array_n;
}

/*------------------------------------------------------------ IN/OUT METHODS */

static void __swap(
    heap_t * hp,
    const int i1,
    const int i2
)
{
    void *tmp = hp->array[i1];

    hp->array[i1] = hp->array[i2];
    hp->array[i2] = tmp;
}

static void __pushup(
    heap_t * hp,
    int idx
)
{
    while (1)
    {
        int parent, compare;

        if (0 == idx)
            return;

        parent = (idx - 1) / 2;
        compare = hp->cmp(hp->array[idx], hp->array[parent], hp->udata);

        /* we are smaller than the parent */
        if (compare < 0)
        {
            return;
        }
        else
        {
            __swap(hp, idx, parent);
        }

        idx = parent;
    }
}

static void __pushdown(
    heap_t * hp,
    int idx
)
{
    while (1)
    {
        int childl, childr, child, compare;

        childl = idx * 2 + 1;
        childr = idx * 2 + 2;
        child = -1;

        assert(idx != hp->count);

        if (childr >= hp->count)
        {
            /* can't pushdown any further */
            if (childl >= hp->count)
                return;

            child = childl;
        }
        else
        {
            /* find biggest child */
            compare = hp->cmp(hp->array[childl], hp->array[childr], hp->udata);

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

        compare = hp->cmp(hp->array[idx], hp->array[child], hp->udata);

        /* idx is smaller than child */
        if (compare < 0)
        {
            assert(hp->array[idx]);
            assert(hp->array[child]);
            __swap(hp, idx, child);
            idx = child;
            /* bigger than the biggest child, we stop, we WIN */
        }
        else
        {
            return;
        }
    }
}

/** Add this value to the heap. */
void heap_offer(
    heap_t * hp,
    void *item
)
{
    assert(hp);
    assert(item);
    if (!item)
        return;

    __ensurecapacity(hp);

    hp->array[hp->count] = item;

    __pushup(hp, hp->count);

    hp->count++;
}

static void DEBUG_check_validity(
    heap_t * hp
)
{
#if DEBUG
    int ii;

    for (ii = 0; ii < hp->count; ii++)
        assert(hp->array[ii]);
#endif
}

/** Remove the top value from this heap. */
void *heap_poll(
    heap_t * hp
)
{
    void *item;

    assert(hp);

    if (!hp)
        return NULL;

    if (0 == heap_count(hp))
        return NULL;

    DEBUG_check_validity(hp);

    item = hp->array[0];

    hp->array[0] = NULL;
    __swap(hp, 0, hp->count - 1);
    hp->count--;

    DEBUG_check_validity(hp);

    if (hp->count > 0)
    {
        assert(hp->array[0]);
        __pushdown(hp, 0);
    }

    DEBUG_check_validity(hp);

    return item;
}

void *heap_peek(
    heap_t * hp
)
{
    if (!hp)
        return NULL;

    if (0 == heap_count(hp))
        return NULL;

    return hp->array[0];
}

/*------------------------------------------------------------ STATUS METHODS */

/**
 * How many elements are there in this heap? */
int heap_count(
    heap_t * hp
)
{
    return hp->count;
}

/*--------------------------------------------------------------79-characters-*/
