#include <stdio.h>

#include "debug.h"
#include "value.h"

void disassembleChunk(Chunk* chunk, const char* name) {
    int offset = 0;
    while (offset < chunk->count) {
        offset = disassembleInstruction(chunk, offset);
    }
}

int disassembleInstruction(Chunk* chunk, uint32_t offset) {
#define TO_STRING(x) #x
#define SIMPLE_INSTRUCTION(op)    simpleInstruction(TO_STRING(op), offset)
#define CONSTANT_INSTRUCTION      constantInstruction(TO_STRING(OP_CONSTANT), offset, chunk)
#define CONSTANT_LONG_INSTRUCTION constantLongInstruction(TO_STRING(OP_CONSTANT_LONG), offset, chunk)

    /* Instruction: "OpCode" "Line" "OpName"                           */
    /* OP_CONSTANT:                          "ConstantsOffset" "Value" */
    ByteWord instruction = chunk->opCode[offset];
    switch (instruction) {
        case INS_RET: CERO_TRACE("Return %d\n", instruction); return offset + 1;
        default:      CERO_TRACE("Unknow opcode %d\n", instruction); return offset + 1;
    }

#undef TO_STRING
#undef SIMPLE_INSTRUCTION
#undef CONSTANT_INSTRUCTION
#undef CONSTANT_LONG_INSTRUCTION
}
