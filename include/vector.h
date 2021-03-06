#ifndef _vector_h_
#define _vector_h_

#include <stdlib.h>
#include <stdbool.h>

typedef struct vector {
    size_t  reserved;
    size_t  size;
    void**  elements;
} vector_t;

void vector_init(vector_t* vector, size_t reserved);

typedef void (*delete_func_t)(void*);

void vector_wipe(vector_t* vector, void (*free_element)(void*));

vector_t* vector_new(size_t reserved);

void vector_delete(vector_t* vector, void (*free_element)(void*));

void vector_insert(vector_t* vector, void* element, size_t index);

void vector_push(vector_t* vector, void* element);

void vector_remove(vector_t* vector, size_t index);

void vector_pop(vector_t* vector);

void* vector_get(const vector_t* vector, size_t index);

void vector_set(vector_t* vector, void* element, size_t index);

void vector_map(vector_t* vector, void (*function)(int, void*));

typedef bool (*cmp_func_t)(const void*, const void*);

bool vector_contains(const vector_t* vector, const void* element,
                     cmp_func_t cmp_func);

void* vector_find(const vector_t* vector, const void* element,
                  cmp_func_t cmp_func);

#if 0
void vector_filter(vector_t* vector, bool (*function)(void*));

void* vector_reduce(vector_t* vector, void* (*function)(void*, void*));
#endif

#endif
