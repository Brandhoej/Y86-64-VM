#ifndef cero_debug_h
#define cero_debug_h

#include "chunk.h"
#include "common.h"
#include "vm.h"

#define DEBUG_TRACE_EXECUTION

void disassembleChunk(Chunk* chunk, const char* name);
int disassembleInstruction(Chunk* chunk, uint32_t offset);

#endif