#include <stdio.h>

#include "memory.h"
#include "value.h"

void initQuadWordArray(QuadWordArray* array) {
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
}

void writeValueArray(QuadWordArray* array, QuadWord value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values   = GROW_ARRAY(QuadWord, array->values, oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(QuadWordArray* array) {
    FREE_ARRAY(QuadWord, array->values, array->capacity);
    initQuadWordArray(array);
}

void printValue(QuadWord value) {
    printf("%g", value);
}