#ifndef HEAP_H
#define HEAP_H

typedef struct heap_s heap_t;

/**
 * Create a new heap and initialise it.
 *
 * malloc()s space for heap.
 *
 * @param[in] cmp Callback used to determine the priority of the item
 * @param[in] udata User data passed through to cmp callback
 * @return initialised heap */
heap_t *heap_new(int (*cmp) (const void *,
                             const void *,
                             const void *udata),
                 const void *udata);

/**
 * Initialise a heap. Use memory provided by user.
 *
 * No malloc()s are performed.
 *
 * @param[in] cmp Callback used to determine the priority of the item
 * @param[in] udata User data passed through to cmp callback
 * @param[in] array Array used for heap
 * @param[in] size The size of the array */
void heap_init(heap_t* h,
               int (*cmp) (const void *,
                           const void *,
                           const void *udata),
               const void *udata,
               unsigned int size);

/**
 * Free memory held by heap */
void heap_free(heap_t * hp);

/**
 * Add item to heap.
 *
 * Ensures that heap can hold item.
 *
 * NOTE:
 *  realloc() possibly called.
 *  The heap pointer will be changed if the heap needs to be enlarged.
 *
 * @param[in/out] hp_ptr Pointer to the heap. Changed when heap is enlarged.
 * @param[in] item Item to be added to the heap
 * @return 0 on success; otherwise -1 on failure */
int heap_offer(heap_t **hp_ptr, void *item);

/**
 * Add item to heap.
 *
 * An error will occur if there isn't enough space in the heap for this item.
 * no malloc()s called.
 *
 * @param[in] item Item to be added to the heap
 * @return 0 on success; -1 on error */
int heap_offerx(heap_t * hp, void *item);

/**
 * Remove the top value from this heap.
 * @return top item of the heap */
void *heap_poll(heap_t * hp);

/**
 * @return top item of the heap */
void *heap_peek(heap_t * hp);

/**
 * Clear all items from the heap.
 * Does not free items. Only use if item memory is managed outside of heap */
void heap_clear(heap_t * hp);

/**
 * @return number of items in heap */
int heap_count(heap_t * hp);

/**
 * @return size of array */
int heap_size(heap_t * hp);

/**
 * @return number of bytes needed for a heap of this size. */
size_t heap_sizeof(unsigned int size);

/**
 * Remove item from heap
 *
 * @param[in] item Item that is to be removed
 * @return item to be removed; NULL if item does not exist */
void *heap_remove_item(heap_t * hp, const void *item);

/**
 * The heap will remove this item
 *
 * @param[in] item Item to be removed
 * @return 1 if the heap contains this item; 0 otherwise */
int heap_contains_item(heap_t * hp, const void *item);

#endif /* HEAP_H */
