#include <stdlib.h>
#include <string.h>
#include "vector.h"

void vector_init(vector_t* vector, size_t reserved) {
    vector->reserved = reserved;
    vector->size     = 0;
    vector->elements = calloc(reserved, sizeof(void*));
}

void vector_wipe(vector_t* vector, void (*free_element)(void*)) {
    if (free_element) {
        for (int i = 0; i < vector->size; i++) {
            free_element(vector->elements[i]);
        }
    }
    if (vector->elements) {
        free(vector->elements);
    }
}

vector_t* vector_new(size_t reserved) {
    vector_t* vector = malloc(sizeof(vector_t));
    vector_init(vector, reserved);
    return vector;
}

void vector_delete(vector_t* vector, void (*free_element)(void*)) {
    vector_wipe(vector, free_element);
    free(vector);
}

static void vector_resize(vector_t* vector) {
    if (vector->reserved == 0) {
        vector->elements = realloc(vector->elements, sizeof(void*));
        vector->reserved = 1;
    } else {
        vector->elements = realloc(vector->elements,
                                   2 * vector->reserved * sizeof(void*));
        memset(vector->elements + vector->reserved,
               0,
               vector->reserved * sizeof(void*));
        vector->reserved *= 2;
    }
}

static void vector_rshift(vector_t* vector, size_t from) {
    while (from >= vector->reserved) {
        vector_resize(vector);
    }
    if (vector->size == vector->reserved) {
        vector_resize(vector);
    }
    if (from < vector->size) {
        memmove(vector->elements + from + 1, vector->elements + from,
                (vector->size - from) * sizeof(void*));
    }
    vector->elements[from] = NULL;
}

void vector_insert(vector_t* vector, void* element, size_t index) {
    vector_rshift(vector, index);
    vector->elements[index] = element;
    if (index > vector->size) {
        vector->size = index + 1;
    } else {
        vector->size++;
    }
}

void vector_push(vector_t* vector, void* element) {
    vector_insert(vector, element, vector->size);
}

static void vector_lshift(vector_t* vector, size_t from) {
    if (from >= vector->size) {
        return;
    }
    memmove(vector->elements + from, vector->elements + from + 1,
            (vector->size - from - 1) * sizeof(void*));
    vector->size--;
}

void vector_remove(vector_t* vector, size_t index) {
    vector_lshift(vector, index);
}

void vector_pop(vector_t* vector) {
    vector_remove(vector, vector->size - 1);
}

void* vector_get(const vector_t* vector, size_t index) {
    return vector->elements[index];
}

void vector_set(vector_t* vector, void* element, size_t index) {
    if (index >= vector->size) {
        vector_insert(vector, element, index);
    } else {
        vector->elements[index] = element;
    }
}

void vector_map(vector_t* vector, void (*function)(int, void*)) {
    for (int i = 0; i < vector->size; i++) {
        function(i, vector->elements[i]);
    }
}

bool vector_contains(const vector_t* vector, const void* element,
                     cmp_func_t cmp_func)
{
    for (int i = 0; i < vector->size; i++) {
        if (cmp_func(vector->elements[i], element)) {
            return true;
        }
    }
    return false;
}

void* vector_find(const vector_t* vector, const void* element,
                  cmp_func_t cmp_func)
{
    for (int i = 0; i < vector->size; i++) {
        if (cmp_func(vector->elements[i], element)) {
            return vector->elements[i];
        }
    }
    return NULL;
}
