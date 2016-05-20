#include <assert.h>
#include "vector.h"

int main(void) {
    vector_t vector;

    vector_init(&vector, 0);

    vector_insert(&vector, (void*)5, 4);
    assert(vector.size == 5);
    assert(vector.elements[4] == (void*)5);

    vector_insert(&vector, (void*)3, 2);
    assert(vector.size == 6);
    assert(vector.elements[2] == (void*)3);
    assert(vector.elements[5] == (void*)5);

    vector_remove(&vector, 2);
    assert(vector.size == 5);
    assert(vector.elements[4] == (void*)5);

    vector_pop(&vector);
    assert(vector.size == 4);

    vector_wipe(&vector, NULL);

    return 0;
}

