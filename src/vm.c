#include <stdio.h>
#include <inttypes.h>

#include "common.h"
#include "debug.h"
#include "vm.h"

void initVM(VM* vm) {
    vm->registers[REG_RAX]  = 0;
    vm->registers[REG_RCX]  = 0;
    vm->registers[REG_RDX]  = 0;
    vm->registers[REG_RRBX] = 0;
    vm->registers[REG_RSP]  = 0;
    vm->registers[REG_RBP]  = 0;
    vm->registers[REG_RSI]  = 0;
    vm->registers[REG_RDI]  = 0;
    vm->registers[REG_R8]   = 0;
    vm->registers[REG_R9]   = 0;
    vm->registers[REG_R10]  = 0;
    vm->registers[REG_R11]  = 0;
    vm->registers[REG_R12]  = 0;
    vm->registers[REG_R13]  = 0;
    vm->registers[REG_R14]  = 0;
    vm->registers[REG_F]    = 0;
    vm->conditionCodes      = 0;
    vm->statusCondition     = STAT_AOK;
}

void freeVM(VM* vm) {

}

void printRegisters(VM* vm) {
    CERO_TRACE("RAX  :: 0x%016" PRId64 "\n", vm->registers[REG_RAX]);
    CERO_TRACE("RCX  :: 0x%016" PRId64 "\n", vm->registers[REG_RCX]);
    CERO_TRACE("RDX  :: 0x%016" PRId64 "\n", vm->registers[REG_RDX]);
    CERO_TRACE("RRBX :: 0x%016" PRId64 "\n", vm->registers[REG_RRBX]);
    CERO_TRACE("RSP  :: 0x%016" PRId64 "\n", vm->registers[REG_RSP]);
    CERO_TRACE("RBP  :: 0x%016" PRId64 "\n", vm->registers[REG_RBP]);
    CERO_TRACE("RSI  :: 0x%016" PRId64 "\n", vm->registers[REG_RSI]);
    CERO_TRACE("RDI  :: 0x%016" PRId64 "\n", vm->registers[REG_RDI]);
    CERO_TRACE("R8   :: 0x%016" PRId64 "\n", vm->registers[REG_R8]);
    CERO_TRACE("R9   :: 0x%016" PRId64 "\n", vm->registers[REG_R9]);
    CERO_TRACE("R10  :: 0x%016" PRId64 "\n", vm->registers[REG_R10]);
    CERO_TRACE("R11  :: 0x%016" PRId64 "\n", vm->registers[REG_R11]);
    CERO_TRACE("R12  :: 0x%016" PRId64 "\n", vm->registers[REG_R12]);
    CERO_TRACE("R13  :: 0x%016" PRId64 "\n", vm->registers[REG_R13]);
    CERO_TRACE("R14  :: 0x%016" PRId64 "\n", vm->registers[REG_R14]);
    CERO_TRACE("F    :: 0x%016" PRId64 "\n", vm->registers[REG_F]);
}

void printFlags(VM* vm) {
    CERO_TRACE("ZF   :: %d\n", (vm->conditionCodes & CC_ZF) >> CC_ZF);
    CERO_TRACE("SF   :: %d\n", (vm->conditionCodes & CC_SF) >> CC_SF);
    CERO_TRACE("OF   :: %d\n", (vm->conditionCodes & CC_OF) >> CC_OF);
}

void printStatus(VM* vm) {
    CERO_TRACE("STAT :: %i\n", vm->statusCondition);
    CERO_TRACE("PC   :: %" PRId16 "\n", vm->pc);
}

void push(VM* vm, QuadWord value) {
    vm->stack[vm->registers[REG_RBP]] = value;
    vm->registers[REG_RBP]++;
}

QuadWord pop(VM* vm) {
    vm->registers[REG_RBP]--;
    return vm->stack[vm->registers[REG_RBP]];
}

void printStack(VM* vm) {
    QuadWord rsb = vm->registers[REG_RBP]; /* Upper bound */
    QuadWord rsi = vm->registers[REG_RSI]; /* Lower bound */
    if (rsb == rsi) {
        CERO_TRACE("[ ]");
    } else {
        CERO_TRACE();
        for (QuadWord i = rsi; i < rsb; i++) {
        CERO_PRINT("[ %" PRId64 " ]", vm->stack[i]);
        }
        CERO_PRINT("\n");
    }
    CERO_PRINT("\n");
}

void run(VM* vm, Chunk* chunk) {
    #define READ_BYTE() (*vm->pc)
    vm->chunk = chunk;
    vm->pc = vm->chunk->opCode;

    while(true) {
        /* disassembleInstruction(vm->chunk, (QuadWord)(vm->pc - vm->chunk->opCode)); */
        ByteWord instruction;
        switch (instruction = READ_BYTE()) {
            case INS_HALT: {
                vm->statusCondition = STAT_HLT;
                return;
            }
            case INS_NOP: break;
            case INS_RRMOVQ: {
                /* Format: 20 rArB       */
                /* Description: rB <- rA */
                /* Fetch                 */
                ByteWord* newPc = vm->pc + 2;
                ByteWord rA     = *(vm->pc + 1) >> 4;
                ByteWord rB     = *(vm->pc + 1) & 0x0F;
                /* Decode                */
                QuadWord valA = vm->registers[rA];
                /* Execute               */
                /* Memory                */
                vm->registers[rB] = valA;
                /* write back            */
                vm->pc = newPc;
            }   break;
            case INS_PUSHQ: {
                /* Format: A0 rAF   */
                /* Fetch            */
                ByteWord* newPc = vm->pc + 2;
                ByteWord rA     = *(vm->pc + 1) >> 4;
                /* Decode           */
                QuadWord val = vm->registers[rA];
                /* Execute          */
                push(vm, val);
                /* Memory           */
                /* write back       */
                vm->pc = newPc;
            } break;
            case INS_POPQ: {
                /* Format: B0 rAF */
                /* Fetch            */
                ByteWord* newPc = vm->pc + 2;
                ByteWord rA    = *(vm->pc + 1) >> 4;
                /* Decode           */
                /* Execute          */
                vm->registers[rA] = pop(vm);
                /* Memory           */
                /* write back       */
                vm->pc = newPc;
            } break;
            default: return;
        }

        printRegisters(vm);
        /* printFlags(vm);     */
    }

    #undef READ_BYTE
}
