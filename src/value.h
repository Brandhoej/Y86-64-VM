#ifndef cero_value_h
#define cero_value_h

#include "common.h"

typedef uint8_t vm_ubyte_t;
typedef int8_t  vm_byte_t;
typedef int16_t vm_word_t;
typedef int32_t vm_double_t;
typedef int64_t vm_quad_t;

/* The constant pool for values.
 * The instruction to load a constant
 * looks up the value by index in that array. */
typedef struct {
    uint32_t capacity;
    uint32_t count;
    vm_quad_t* values;
} QuadWordArray;



void initQuadWordArray(QuadWordArray* array);
void writeValueArray(QuadWordArray* array, vm_quad_t value);
void freeValueArray(QuadWordArray* array);

void printValue(vm_quad_t value);

#endif