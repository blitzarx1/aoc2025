#ifndef ARRAY_H
#define ARRAY_H

#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "array"
#include "log.h"

#include <stdlib.h>
#include <string.h>

#define ARR_DEFAULT_CAP 2
#define ARR_CAP_MULT 2

typedef struct {
    void *vals;
    size_t  valsize;
    size_t  len;
    size_t  cap;
} Array;

Array *arrnew(size_t valsize, size_t cap);
int arrappend(Array *arr, const void *val);
size_t arrlen(const Array *arr);
void *arrget(const Array *arr, size_t idx);
void arrfree(Array *arr);

// Helper macro for typed access returning pointer to value
#define ARR_GET(type, arr, idx)     ((type *)(arrget((arr), (idx))))
// Helper macro for typed access returning value
#define ARR_GET_VAL(type, arr, idx) (*(type *)(arrget((arr), (idx))))

#ifdef ARRAY_IMPLEMENTATION

static int arralloc(Array *arr, size_t cap) {
    size_t memsize = arr->valsize * cap;
    LOG_DEBUG("allocating %zu bytes for cap %zu", memsize, cap);

    void *vals = malloc(memsize);
    if (!vals) {
        LOG_ERROR("malloc failed");
        return -1;
    }

    arr->vals = vals;
    arr->cap = cap;
    return 0;
}

Array *arrnew(size_t valsize, size_t cap) {
    LOG_DEBUG("called with valsize %zu cap %zu", valsize, cap);

    if (valsize == 0) {
        LOG_ERROR("called with valsize=0");
        return NULL;
    }

    Array *arr = malloc(sizeof(Array));
    *arr = (Array){ NULL, valsize, 0, 0 };

    if (cap > 0) {
        if (arralloc(arr, cap) < 0) {
            return NULL;
        }
    }

    return arr;
}

int arrappend(Array *arr, const void *val) {
    if (!arr || !val || arr->valsize == 0) {
        LOG_ERROR("invalid args (arr=%p val=%p valsize=%zu)",
            (void *)arr, val, arr ? arr->valsize : 0);
        return -1;
    }

    if (arr->cap == 0) {
        if (arralloc(arr, ARR_DEFAULT_CAP) < 0) {
            return -1;
        }
    }

    if (arr->len + 1 > arr->cap) {
        size_t new_cap = arr->cap * ARR_CAP_MULT;
        size_t memsize = arr->valsize * new_cap;
        LOG_DEBUG("realloc to %zu bytes (new cap %zu)", memsize, new_cap);

        void *new_vals = realloc(arr->vals, memsize);
        if (!new_vals) {
            LOG_ERROR("realloc failed");
            return -1;
        }

        arr->vals = new_vals;
        arr->cap = new_cap;
    }

    unsigned char *base = (unsigned char *)arr->vals;
    memcpy(base + arr->len * arr->valsize, val, arr->valsize);
    arr->len++;

    return 0;
}

size_t arrlen(const Array *arr) {
    return arr ? arr->len : 0;
}

void *arrget(const Array *arr, size_t idx) {
    if (!arr) {
        LOG_ERROR("called with NULL arr");
        return NULL;
    }
    if (idx >= arr->len) {
        LOG_ERROR("index %zu out of bounds (len %zu)", idx, arr->len);
        return NULL;
    }

    unsigned char *addr =
        (unsigned char *)arr->vals + idx * arr->valsize;

    LOG_DEBUG("got element addres with idx=%zu: %p", idx, (void *)addr);

    return addr;
}

void arrfree(Array *arr) {
    if (!arr) return;
    LOG_DEBUG("freeing len=%zu cap=%zu", arr->len, arr->cap);
    free(arr->vals);
    free(arr);
}

#endif // ARRAY_IMPLEMENTATION
#endif // ARRAY_H
