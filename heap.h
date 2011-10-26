
typedef struct
{
    void **array;
    int arraySize;
    int count;
    /**  user data */
    void *udata;
    int (
    *cmp
    )   (
    const void *,
    const void *,
    const void *
    );
} heap_t;

heap_t *heap_new(
    int (*cmp) (const void *,
                const void *,
                const void *udata),
    const void *udata
);

void heap_free(
    heap_t * hp
);

void heap_offer(
    heap_t * hp,
    void *item
);

void *heap_poll(
    heap_t * hp
);

void *heap_peek(
    heap_t * hp
);

int heap_count(
    heap_t * hp
);
