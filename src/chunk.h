#ifndef cero_chunk_h
#define cero_chunk_h

#include "common.h"
#include "value.h"

/* Encodings                                          */
#define REG_SPEC_ENC_RARB(rA, rB) ((rA << 4) | rB)    
#define REG_SPEC_ENC_RA(ra) (REG_SPEC_ENC_RARB(ra, REG_F))
#define REG_SPEC_ENC_RB(rb) (REG_SPEC_ENC_RARB(REG_F, rb))

/* Decodings                                     */
#define REG_SPEC_DEC_RA(regSpec) (regSpec >> 4)  
#define REG_SPEC_DEC_RB(regSpec) (0xF0 & regSpec)

typedef struct {
    ByteWord count;
    ByteWord capacity;
    ByteWord* opCode;
    QuadWordArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, ByteWord bytes[], int length);

/* name + "Write" -- This way the naming follows the specifications */
void haltWrite(Chunk* chunk);
void nopWrite(Chunk* chunk);
void rrmovqWrite(Chunk* chunk, ByteWord rA, ByteWord rB);
void irmovqWrite(Chunk* chunk, ByteWord rB, QuadWord v);
void pushqWrite(Chunk* chunk, ByteWord rA);
void popqWrite(Chunk* chunk, ByteWord rA);

#endif