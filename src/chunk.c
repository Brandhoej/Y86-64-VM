#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "chunk.h"
#include "memory.h"
#include "vm.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->opCode = NULL;
    initQuadWordArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
    /* By only freeing the chunk operation code sequence
     * we can reuse our chunks by initialising them afterwards */
    FREE_ARRAY(ByteWord, chunk->opCode, chunk->capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk* chunk, ByteWord bytes[], int length) {
    /* If the chunk can *not* store one more byte-code instruction 
     * (opCode) then we have to grow the chunk capcity.
     * This is also the case at the first write, because the array
     * is NULL and capcity is 0. */
    if (chunk->capacity < chunk->count + length) {
        int oldCapcity  = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapcity);
        chunk->opCode   = GROW_ARRAY(ByteWord, chunk->opCode, oldCapcity, chunk->capacity);
        /* By recursive calling we ensure that the capcity will fit the bytes */
        writeChunk(chunk, bytes, length);
    } else {
        for (int i = 0; i < length; i++) {
            chunk->opCode[chunk->count] = bytes[i];
            chunk->count++;
        }
    }
}

void haltWrite(Chunk* chunk) {
    ByteWord bytes[1];
    bytes[0] = INS_HALT;
    writeChunk(chunk, bytes, 1);
}

void nopWrite(Chunk* chunk) {
    ByteWord bytes[1];
    bytes[0] = INS_NOP;
    writeChunk(chunk, bytes, 1);
}

void rrmovqWrite(Chunk* chunk, ByteWord rA, ByteWord rB) {
    ByteWord bytes[2];
    bytes[0] = INS_RRMOVQ;
    bytes[1] = REG_SPEC_ENC_RARB(rA, rB);
    writeChunk(chunk, bytes, 2);
}

void irmovqWrite(Chunk* chunk, ByteWord rB, QuadWord v) {
    ByteWord bytes[10];
    bytes[0] = INS_IRMOVQ;
    bytes[1] = REG_SPEC_ENC_RB(rB);
    for (int i = 0; i < sizeof(v); ++i) {
        bytes[2 + i] = (v >> (i * 4)) & 0xFF;
    }
}

void pushqWrite(Chunk* chunk, ByteWord rA) {
    ByteWord bytes[2];
    bytes[0] = INS_PUSHQ;
    bytes[1] = REG_SPEC_ENC_RA(rA);
    writeChunk(chunk, bytes, 2);
}

void popqWrite(Chunk* chunk, ByteWord rA) {
    ByteWord bytes[2];
    bytes[0] = INS_POPQ;
    bytes[1] = REG_SPEC_ENC_RA(rA);
    writeChunk(chunk, bytes, 2);
}