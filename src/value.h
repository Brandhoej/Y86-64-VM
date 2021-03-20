#ifndef cero_value_h
#define cero_value_h

#include "common.h"

typedef uint8_t ByteWord;
typedef uint16_t WordWord;
typedef uint32_t DoubleWord;
typedef uint64_t QuadWord;


/* The constant pool for values.
 * The instruction to load a constant
 * looks up the value by index in that array. */
typedef struct {
    uint32_t capacity;
    uint32_t count;
    QuadWord* values;
} QuadWordArray;



void initQuadWordArray(QuadWordArray* array);
void writeValueArray(QuadWordArray* array, QuadWord value);
void freeValueArray(QuadWordArray* array);

void printValue(QuadWord value);

#endif