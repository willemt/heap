/*
*  C Implementation: tea_heap
*
* Description: 
*
*
* Author: Willem-Hendrik Thiart <beandaddy@proxima>, (C) 2006
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include "tea_local.h"
//#include "tea_utils.h"
#include "tea_datatype.h"

#if 0
static tea_iface_adt_t iface = {
	(adtAdd_f)tea_object_t, /* add */
	(adtRemove_f)tea_alistf_remove, /* remove */
	(adtInsert_f)tea_alistf_insert, /* insert */
	(adtGet_f)tea_alistf_get, /* get */
	(adtCount_f)tea_alistf_size, /* count */
	(adtIsEmpty_f)tea_alistf_isEmpty, /* isEmpty */
	(tea_adt_iter_f)tea_alistf_iter, /* iter */
	NULL, /* sort */
	(adtClear_f)tea_alistf_empty, /* empty */
	(adtItemContains_f)tea_alistf_contains, /* itemContains */
	(adtItemGet_f)tea_alistf_getItem, /* itemGet */
	(adtItemRemove_f)tea_alistf_removeItem, /* itemRemove */
	(adtItemIndex_f)tea_alistf_itemIndex, /* itemIndex */
	(adtFree_f)tea_alistf_free, /* free */
	(adtFreeAll_f)tea_alistf_freeall, /* freeAll */
	(adtInit_f)tea_alistf_init, /* init */
	(adtInitAlloc_f)tea_alistf_initalloc, /* initAlloc */
};
#endif

typedef struct {
	tea_object_t	obj;
	int		arraySize;
	int		count;

	tea_hashmap_t*	hash; // for the contains() method !!! !!!
//	tea_heap_t*	spareHeap;
} heap_in_t;

#define DEBUG 0
#define INITIAL_CAPACITY 16
#define in(x) ((heap_in_t*)x->inside)

/*-------------------------------------------------------------- INIT METHODS */

/** Init a heap. */
void tea_heap_init(tea_heap_t* hp, tea_object_t* object)
{
	hp->inside = tea_malloc_zero(sizeof(heap_in_t));
	in(hp)->arraySize = INITIAL_CAPACITY;
	hp->array = tea_malloc(sizeof(void*) * in(hp)->arraySize);
	if (object)
		memcpy(&in(hp)->obj, object, sizeof(tea_object_t));
//	in(hp)->spareHeap = NULL;
}

/** Init a heap and return it.
 * We malloc space for it. */
tea_heap_t* tea_heap_initalloc(tea_object_t* object)
{
	tea_heap_t*	hp = tea_malloc(sizeof(tea_heap_t));

	tea_heap_init(hp, object);
	
	return hp;
}

void tea_heap_free(tea_heap_t* hp)
{
//	tea_heap_freeall(in(hp)->spareHeap);
    if (in(hp)->hash)
        tea_hashmap_freeall(in(hp)->hash);
	free(hp->inside);
}

void tea_heap_freeall(tea_heap_t* hp)
{
	if (!hp) return;
//	assert(hp);
//	teaStub();
	tea_heap_free(hp);
}

static void __ensurecapacity(tea_heap_t* hp)
{
	int ii;
	void** array_n;

	if (in(hp)->count < in(hp)->arraySize) return;

	in(hp)->arraySize *= 2;

//	tea_msg(mSYS, 1, "enlargining heap: %d -> %d\n",
//			in(hp)->arraySize, in(hp)->arraySize); 
    
    array_n = tea_malloc(sizeof(void*) * in(hp)->arraySize);

	for (ii=0; ii<in(hp)->count; ii++)
    {
		array_n[ii] = hp->array[ii];
        assert(array_n[ii]);
    }

	free(hp->array);
	hp->array = array_n;
}

/*------------------------------------------------------------ IN/OUT METHODS */

static void __swap(tea_heap_t* hp, int i1, int i2)
{
//    printf("%d %d\n", i1, in(hp)->count);
//    assert(i1 < in(hp)->count);
//    assert(i2 < in(hp)->count);
	void* tmp = hp->array[i1];
	hp->array[i1] = hp->array[i2];
	hp->array[i2] = tmp;
}

static void __pushup(tea_heap_t* hp, int idx)
{
	while (TRUE)
    {
		int parent, compare;

		if (0 == idx)
            return;

        parent = (idx-1)/2;
        compare = in(hp)->obj.compare(hp->array[idx], hp->array[parent]); 

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

static void __pushdown(tea_heap_t* hp, int idx)
{
	while (TRUE)
    {
		int childl = idx * 2 + 1;
		int childr = idx * 2 + 2;
		int child = -1;
		int compare;

//		printf("(idx:%d cl:%d cr:%d) size:%d\n",
//		    idx,childl,childr,in(hp)->count);

        assert(idx != in(hp)->count);
/*		if (idx >= in(hp)->count) {
			return;
		}
*/
		if (childr >= in(hp)->count)
        {
            /* can't pushdown any further */
			if (childl >= in(hp)->count)
				return;

			child = childl;
		}
        else
        {
			/* find biggest child */
			compare = in(hp)->obj.compare(hp->array[childl],hp->array[childr]); 

			if (compare < 0) {
				child = childr;
			} else {
				child = childl;
			}
		}

        assert(child != -1);

//        printf("in(hp) %d\n", in(hp)->count);

		compare = in(hp)->obj.compare(hp->array[idx], hp->array[child]); 

		/* idx is smaller than child */
		if (compare < 0)
        {
            assert(hp->array[idx]);
            assert(hp->array[child]);
			__swap(hp,idx,child);
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
void tea_heap_offer(tea_heap_t* hp, void* item)
{
	assert(hp);
    assert(item);
	if (!item)
        return;

	__ensurecapacity(hp);
	
	hp->array[in(hp)->count] = item;

	__pushup(hp, in(hp)->count);

	/* optional: add to hash */
	if (in(hp)->hash)
		tea_hashmap_put(in(hp)->hash,item,item);

/*	int ii;
	for (ii=0; ii<in(hp)->count; ii++) {
		tea_printf("%d|",ii);
//		hp->array[in(hp)->count]
	}
	tea_printf("\n");
*/
	in(hp)->count++;
}

static void DEBUG_check_validity(tea_heap_t* hp)
{
#if DEBUG
    int ii;
    for (ii=0; ii<in(hp)->count; ii++)
        assert(hp->array[ii]);
#endif
}

/** Remove the top value from this heap. */
void* tea_heap_poll(tea_heap_t* hp)
{
	void* item;

    assert(hp);

	if (!hp)
        return NULL;

	if (tea_heap_isEmpty(hp))
        return NULL;

    DEBUG_check_validity(hp);
    
    item = hp->array[0];

	hp->array[0] = NULL;
	__swap(hp, 0, in(hp)->count-1);
	in(hp)->count--;

    DEBUG_check_validity(hp);

//    assert(in(hp)->count > 0);
    if (in(hp)->count > 0)
    {
        assert(hp->array[0]);
        __pushdown(hp, 0);
    }

	if (in(hp)->hash)
		tea_hashmap_remove(in(hp)->hash, item);

    DEBUG_check_validity(hp);

//	printf("polling %s\n", item);
//	in(hp)->obj.print(item);

	return item;
}

void* tea_heap_peek(tea_heap_t* hp)
{
	if (!hp)
        return NULL;

	if (tea_heap_isEmpty(hp))
        return NULL;

	return hp->array[0];
}

/*------------------------------------------------------------ STATUS METHODS */

/** How many elements are there in this heap? */
int tea_heap_size(tea_heap_t* hp)
{
	return in(hp)->count;
}

int tea_heap_isEmpty(tea_heap_t* hp)
{
	return 0 == in(hp)->count;
}

/*------------------------------------------------------------------ ITERATOR */

static int __has_next(void *data)
{
	const tea_iter_t* iter = data;
	tea_heap_t* hp = iter->data;

	if (NULL == hp) return FALSE;

	if (iter->current == in(hp)->count) {
		return FALSE;
	}
	return TRUE;
}

static void* __next(void *data)
{
	tea_iter_t* iter = data;
	const tea_heap_t* hp = iter->data;

	if (!__has_next(data)) return NULL;

	return hp->array[iter->current++];
}

tea_iter_t tea_heap_iterator(tea_heap_t* hp)
{
	assert(hp);
	tea_iter_t iter;
	iter.data = hp;
	iter.next = __next;
	iter.has_next = __has_next;
	iter.current = 0;
	return iter;
}

/*
 * ==============================
 *  || | | | | | HASHING | | | ||
 * ==============================
 */

static void heap_makehashtable(tea_heap_t* hp)
{
	heap_in_t* hpin = hp->inside;

	hpin->hash = tea_hashmap_initalloc(&hpin->obj);//list->obj.hash,list->obj.compare);

	tea_iter_t iter = tea_heap_iterator(hp);
	while (iter.has_next(&iter)) {
		void* tmp = iter.next(&iter);
		tea_hashmap_put(hpin->hash,tmp,tmp);
	}
}

/** Is this item, represented by key inside this arraylist?
 * @retuns 1 if it is, otherwise 0 (also 0 if the object doesn't have a hash code */
int tea_heap_contains(tea_heap_t* hp, void* key)
{
	heap_in_t* hpin = hp->inside;

	if (!hpin->hash && hpin->obj.hash) {
		heap_makehashtable(hp);
	}
	
	if (tea_hashmap_containsKey(hpin->hash,key)) {
		return 1;
	}
	
	return 0;
}
