#include <stdio.h>

#include "memory.h"
#include "value.h"

void initQuadWordArray(QuadWordArray* array) {
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
}

void writeValueArray(QuadWordArray* array, vm_quad_t value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values   = GROW_ARRAY(vm_quad_t, array->values, oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(QuadWordArray* array) {
    FREE_ARRAY(vm_quad_t, array->values, array->capacity);
    initQuadWordArray(array);
}

void printValue(vm_quad_t value) {
    printf("%g", value);
}