// TODO more comments overall
// TODO static
// TODO no dependency
// TODO doxygen's format comments on header
// TODO thread safe

#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE align(sizeof(struct s_block))

void *global_base = NULL;

static inline size_t align(size_t x) {
    const size_t a = _Alignof(max_align_t);
    return (x + (a - 1)) & ~(a - 1);
}

struct s_block {
    size_t size;
    struct s_block *next;
    struct s_block *prev;
    int free;
    void *ptr;
    char data[]; /* To simplify pointer arithmatic, to be sure that all our
                    operations are done with one byte precision */
};

typedef struct s_block *t_block;

// TODO which functions should be inline?

t_block get_block(void *p) {
    if (!p)
        return NULL;
    return (t_block)((char *)p - BLOCK_SIZE);
}

int is_valid_addr(void *p) {
    if (global_base && p) {
        if (p > global_base && p < sbrk(0)) {
            return p == (get_block(p)->ptr);
        }
    }
    return 0;
}

void split_block(t_block b, size_t s) {
    // Only if there is atleast 4 bytes of space can be created
    if (b->size >= s + BLOCK_SIZE + 4) {
        t_block new;
        new = (t_block)(b->data + s);
        new->size = b->size - s - BLOCK_SIZE;
        new->next = b->next;
        new->prev = b;
        new->free = 1;
        new->ptr = new->data;
        b->size = s;
        b->next = new;
        if (new->next) {
            new->next->prev = new;
        }
    }
}

t_block find_block(t_block *last, size_t size) {
    t_block b = global_base;
    while (b && (!b->free || b->size < size)) {
        *last = b;
        b = b->next;
    }
    return b;
}

t_block extend_heap(t_block last, size_t size) {
    t_block b;
    b = sbrk(0);
    if (sbrk(BLOCK_SIZE + size) == (void *)-1)
        return NULL;
    b->size = size;
    b->next = NULL;
    b->prev = last;
    b->ptr = b->data;
    if (last)
        last->next = b;
    b->free = 0;
    return b;
}

void *malloc(const size_t size) {
    size_t s = align(size);
    t_block b;
    t_block last;
    if (global_base) {
        last = global_base;
        b = find_block(&last, s);
        if (b) {
            split_block(b, s);
            b->free = 0;
        } else {
            b = extend_heap(last, s);
            if (!b) {
                return NULL;
            }
        }
    } else {
        b = extend_heap(NULL, s);
        if (!b) {
            return NULL;
        }
        global_base = b;
    }
    return b->data;
}

void *calloc(size_t nmemb, size_t size) {
    if (size != 0 && nmemb > SIZE_MAX / size) {
        return NULL;
    }
    size_t *new = malloc(nmemb * size);
    if (!new) {
        return NULL;
    }
    memset(new, 0, nmemb * size);
    return new;
}

t_block fuse(t_block b) {
    if (b->next && b->next->free) {
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;
        if (b->next) {
            b->next->prev = b;
        }
    }
    return b;
}

void free(void *ptr) {
    if (is_valid_addr(ptr)) {
        t_block b = get_block(ptr);
        b->free = 1;
        if (b->prev && b->prev->free) {
            b = fuse(b->prev);
        }
        if (b->next) {
            fuse(b);
        } else {
            if (b->prev) {
                b->prev->next = NULL;
            } else {
                global_base = NULL;
            }
            brk(b);
        }
    }
}

void *realloc(void *ptr, size_t size) {
    size = align(size);
    if (!ptr) {
        return malloc(size);
    }
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    if (is_valid_addr(ptr)) {
        t_block b = get_block(ptr);
        size_t old_size = b->size;

        // Already big enough
        if (old_size >= size) {
            split_block(b, size);
        } else {
            // Try fuse with next if possible
            if (b->next && b->next->free &&
                (b->size + BLOCK_SIZE + b->next->size) >= size) {
                fuse(b);
                split_block(b, size);
            } else {
                // Fallback to malloc
                void *new_ptr = malloc(size);
                if (!new_ptr) {
                    return NULL;
                }

                memcpy(new_ptr, ptr, old_size);
                free(ptr);
                return new_ptr;
            }
        }
        return ptr;
    }
    return NULL;
}

void *reallocf(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (!new_ptr)
        free(ptr);
    return new_ptr;
}
