#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#include "common.h"
#include "vm.h"
#include "memory.h"
#include "writer.h"
#include "printer.h"

void initVM(VM* vm) {
    vm->registers = NULL;
    vm->memory    = NULL;
    vm->registers = INIT_ARRAY(vm_quad_t, vm->registers, REG_COUNT);
    vm->memory    = INIT_ARRAY(vm_byte_t, vm->memory, MEM_MAX);
    vm->pc        = 0;
    vm->registers[REG_RBP] = MEM_MAX;
    vm->registers[REG_RSP] = vm->registers[REG_RBP];
    vm->statusCondition    = STAT_AOK;
}

void freeVM(VM* vm) {

}

bool inline zf(VM* vm) {
    return (vm->conditionCodes & 0b0001) >> CC_ZF;
}

bool inline sf(VM* vm) {
    return (vm->conditionCodes & 0b0010) >> CC_SF;
}

bool inline of(VM* vm) {
    return (vm->conditionCodes & 0b0100) >> CC_OF;
}

static bool inline addrInHighMem(VM* vm, vm_quad_t addr) {
    vm_quad_t rsp = vm->registers[REG_RSP];
    return addr >= rsp;
}

static bool inline addrInLowMem(VM* vm, vm_quad_t addr) {
    vm_quad_t rsp = vm->registers[REG_RSP];
    return addr < rsp;
}

vm_ubyte_t inline m1r(VM* vm, vm_quad_t offset) {
    return vm->memory[offset];
}

vm_quad_t inline m8r(VM* vm, vm_quad_t offset) {
    vm_quad_t quad = 0;
    quad |= m1r(vm, offset + 0); quad <<= 8;
    quad |= m1r(vm, offset + 1); quad <<= 8;
    quad |= m1r(vm, offset + 2); quad <<= 8;
    quad |= m1r(vm, offset + 3); quad <<= 8;
    quad |= m1r(vm, offset + 4); quad <<= 8;
    quad |= m1r(vm, offset + 5); quad <<= 8;
    quad |= m1r(vm, offset + 6); quad <<= 8;
    quad |= m1r(vm, offset + 7); quad <<= 0;
    return quad;
}

vm_ubyte_t m1w(VM* vm, vm_quad_t offset, vm_ubyte_t byte) {
    vm->memory[offset] = byte;
}

vm_quad_t m8w(VM* vm, vm_quad_t offset, vm_quad_t quad) {
    m1w(vm, offset + 7, (quad >>  0) & 0xFF);
    m1w(vm, offset + 6, (quad >>  8) & 0xFF);
    m1w(vm, offset + 5, (quad >> 16) & 0xFF);
    m1w(vm, offset + 4, (quad >> 24) & 0xFF);
    m1w(vm, offset + 3, (quad >> 32) & 0xFF);
    m1w(vm, offset + 2, (quad >> 40) & 0xFF);
    m1w(vm, offset + 1, (quad >> 48) & 0xFF);
    m1w(vm, offset + 0, (quad >> 56) & 0xFF);
}

/* -----Information about standards-----
 * Fetch:   Read instruction from memory
 * Decode:  Read program registers
 * Execute: Compute value or address
 * Memory:  Read or write data
 * Write    Back: write program registers
 * PC:      Update program counter
 * 
 * Instruction format (10 bytes max)
 * Instruction byte:       icode:ifun
 * Optional register byte: rA:rB
 * Optional constant word: valC
 * 
 * https://w3.cs.jmu.edu/lam2mo/cs261_2018_08/files/y86-isa.pdf
 */

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  00                              */
static inline void halt(VM* vm) {
    CERO_DEBUG("ins::halt\n");
    /* Fetch      */
    vm_quad_t valP = vm->pc + 1;
    /* Decode     */
    /* Execute    */
    /* Memory     */
    /* Write back */
    vm->statusCondition = STAT_HLT;
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  10                              */
static inline void nop(VM* vm) {
    CERO_DEBUG("ins::nop\n");
    /* Fetch      */
    vm_quad_t valP = vm->pc + 1;
    /* Decode     */
    /* Execute    */
    /* Memory     */
    /* Write back */
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  20 AB                           */
static inline void rrmovq(VM* vm) {
    CERO_DEBUG("ins::rrmovq\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valP  = vm->pc + 2;
    /* Decode     */
    vm_quad_t valB  = vm->registers[rB];
    /* Execute    */
    /* Memory     */
    /* Write back */
    vm->registers[rA] = valB;
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  30 FB -----------V-----------   */
static inline void irmovq(VM* vm) {
    CERO_DEBUG("ins::irmovq\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valC  = m8r(vm, vm->pc + 2);
    vm_quad_t valP  = vm->pc + 10;
    /* Decode     */
    /* Execute    */
    vm_quad_t valE = valC;
    /* Memory     */
    /* Write back */
    vm->registers[rB] = valE;
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  40 AB -----------D-----------   */
static inline void rmmovq(VM* vm) {
    CERO_DEBUG("ins::rmmovq\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valC  = m8r(vm, vm->pc + 2);
    vm_quad_t valP  = vm->pc + 10;
    /* Decode     */
    vm_quad_t valA  = vm->registers[rA];
    vm_quad_t valB  = vm->registers[rB];
    /* Execute    */
    vm_quad_t valE  = valA + valC;
    CERO_DEBUG("valE %" PRId64 "\n", valE);
    /* Memory     */
    if (addrInHighMem(vm, valE) || addrInHighMem(vm, valE + 8)) {
        vm->statusCondition = STAT_ADR;
        return;
    }
    m8w(vm, valE, valB);
    /* Write back */
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  50 AB -----------D-----------   */
static inline void mrmovq(VM* vm) {
    CERO_DEBUG("ins::mrmovq\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valC  = m8r(vm, vm->pc + 2);
    vm_quad_t valP  = vm->pc + 10;
    /* Decode     */
    vm_quad_t valA  = vm->registers[rA];
    vm_quad_t valB  = vm->registers[rB];
    /* Execute    */
    vm_quad_t valE  = valB + valC;
    /* Memory     */
    vm_quad_t valM  = m8r(vm, valE);
    /* Write back */
    vm->registers[rA] = valM;
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  60 AB                           */
static inline void addq(VM* vm) {
    CERO_DEBUG("ins::addq\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valP  = vm->pc + 2;
    /* Decode     */
    vm_quad_t valA  = vm->registers[rA];
    vm_quad_t valB  = vm->registers[rB];
    /* Execute    */
    vm_quad_t valE = valB + valA;
    vm->conditionCodes |=
        (valE == 0 ? 1 : 0) << CC_ZF |
        (valE < 0  ? 1 : 0) << CC_SF |
        (((valA > 0 && valB > INT64_MAX - valA) || (valA < 0 && valB < INT64_MIN - valA)) ? 1 : 0) << CC_OF;
    /* Memory     */
    /* Write back */
    vm->registers[rB] = valE;
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  61 AB                           */
static inline void subq(VM* vm) {
    CERO_DEBUG("ins::subq\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valP  = vm->pc + 2;
    /* Decode     */
    vm_quad_t valA  = vm->registers[rA];
    vm_quad_t valB  = vm->registers[rB];
    /* Execute    */
    vm_quad_t valE = valB - valA;
    vm->conditionCodes |=
        (valE == 0 ? 1 : 0) << CC_ZF |
        (valE < 0  ? 1 : 0) << CC_SF |
        (((valA > 0 && valB > INT64_MAX - valA) || (valA < 0 && valB < INT64_MIN - valA)) ? 1 : 0) << CC_OF;
    /* Memory     */
    /* Write back */
    vm->registers[rB] = valE;
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  62 AB                           */
static inline void andq(VM* vm) {
    CERO_DEBUG("ins::andq\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valP  = vm->pc + 2;
    /* Decode     */
    vm_quad_t valA  = vm->registers[rA];
    vm_quad_t valB  = vm->registers[rB];
    /* Execute    */
    vm_quad_t valE = valB & valA;
    vm->conditionCodes |=
        (valE == 0 ? 1 : 0) << CC_ZF |
        (valE < 0  ? 1 : 0) << CC_SF |
        (((valA > 0 && valB > INT64_MAX - valA) || (valA < 0 && valB < INT64_MIN - valA)) ? 1 : 0) << CC_OF;
    /* Memory     */
    /* Write back */
    vm->registers[rB] = valE;
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  63 AB                           */
static inline void xorq(VM* vm) {
    CERO_DEBUG("ins::xorq\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valP  = vm->pc + 2;
    /* Decode     */
    vm_quad_t valA  = vm->registers[rA];
    vm_quad_t valB  = vm->registers[rB];
    /* Execute    */
    vm_quad_t valE = valB ^ valA;
    vm->conditionCodes |=
        (valE == 0 ? 1 : 0) << CC_ZF |
        (valE < 0  ? 1 : 0) << CC_SF |
        (((valA > 0 && valB > INT64_MAX - valA) || (valA < 0 && valB < INT64_MIN - valA)) ? 1 : 0) << CC_OF;
    /* Memory     */
    /* Write back */
    vm->registers[rB] = valE;
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  70 ---------Dest----------      */
static inline void jmp(VM* vm) {
    CERO_DEBUG("ins::jmp\n");
    /* Fetch      */
    vm_quad_t valC  = m8r(vm, vm->pc + 1);
    vm_quad_t valP  = vm->pc + 9;
    /* Decode     */
    /* Execute    */
    bool cnd = true;
    /* Memory     */
    /* Write back */
    /* PC update  */
    vm->pc = cnd ? valC : valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  71 ---------Dest----------      */
static inline void jle(VM* vm) {
    CERO_DEBUG("ins::jle\n");
    /* Fetch      */
    vm_quad_t valC  = m8r(vm, vm->pc + 1);
    vm_quad_t valP  = vm->pc + 9;
    /* Decode     */
    /* Execute    */
    bool cnd = sf(vm) | zf(vm);
    /* Memory     */
    /* Write back */
    /* PC update  */
    vm->pc = cnd ? valC : valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  72 ---------Dest----------      */
static inline void jl(VM* vm) {
    CERO_DEBUG("ins::jl\n");
    /* Fetch      */
    vm_quad_t valC  = m8r(vm, vm->pc + 1);
    vm_quad_t valP  = vm->pc + 9;
    /* Decode     */
    /* Execute    */
    bool cnd = sf(vm);
    /* Memory     */
    /* Write back */
    /* PC update  */
    vm->pc = cnd ? valC : valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  73 ---------Dest----------      */
static inline void je(VM* vm) {
    CERO_DEBUG("ins::je\n");
    /* Fetch      */
    vm_quad_t valC  = m8r(vm, vm->pc + 1);
    vm_quad_t valP  = vm->pc + 9;
    /* Decode     */
    /* Execute    */
    bool cnd = zf(vm);
    /* Memory     */
    /* Write back */
    /* PC update  */
    vm->pc = cnd ? valC : valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  74 ---------Dest----------      */
static inline void jne(VM* vm) {
    CERO_DEBUG("ins::jne\n");
    /* Fetch      */
    vm_quad_t valC  = m8r(vm, vm->pc + 1);
    vm_quad_t valP  = vm->pc + 9;
    /* Decode     */
    /* Execute    */
    bool cnd = !zf(vm);
    /* Memory     */
    /* Write back */
    /* PC update  */
    vm->pc = cnd ? valC : valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  75 ---------Dest----------      */
static inline void jge(VM* vm) {
    CERO_DEBUG("ins::jge\n");
    /* Fetch      */
    vm_quad_t valC  = m8r(vm, vm->pc + 1);
    vm_quad_t valP  = vm->pc + 9;
    /* Decode     */
    /* Execute    */
    bool cnd = !sf(vm);
    /* Memory     */
    /* Write back */
    /* PC update  */
    vm->pc = cnd ? valC : valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  76 ---------Dest----------      */
static inline void jg(VM* vm) {
    CERO_DEBUG("ins::jg\n");
    /* Fetch      */
    vm_quad_t valC  = m8r(vm, vm->pc + 1);
    vm_quad_t valP  = vm->pc + 9;
    /* Decode     */
    /* Execute    */
    bool cnd = !sf(vm) & !zf(vm);
    /* Memory     */
    /* Write back */
    /* PC update  */
    vm->pc = cnd ? valC : valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  21 AB                           */
static inline void cmovle(VM* vm) {
    CERO_DEBUG("ins::cmovle\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valP = vm->pc + 2;
    /* Decode     */
    vm_quad_t valA = vm->registers[rA];
    /* Execute    */
    vm_quad_t valE = valA;
    bool cnd = sf(vm) | zf(vm);
    /* Memory     */
    /* Write back */
    if (cnd) {
        vm->registers[rB] = valE;
    }
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  22 AB                           */
static inline void cmovl(VM* vm) {
    CERO_DEBUG("ins::cmovl\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valP = vm->pc + 2;
    /* Decode     */
    vm_quad_t valA = vm->registers[rA];
    /* Execute    */
    vm_quad_t valE = valA;
    bool cnd = sf(vm);
    /* Memory     */
    /* Write back */
    if (cnd) {
        vm->registers[rB] = valE;
    }
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  23 AB                           */
static inline void cmove(VM* vm) {
    CERO_DEBUG("ins::cmove\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valP = vm->pc + 2;
    /* Decode     */
    vm_quad_t valA = vm->registers[rA];
    /* Execute    */
    vm_quad_t valE = valA;
    bool cnd = zf(vm);
    /* Memory     */
    /* Write back */
    if (cnd) {
        vm->registers[rB] = valE;
    }
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  24 AB                           */
static inline void cmovne(VM* vm) {
    CERO_DEBUG("ins::cmovne\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valP = vm->pc + 2;
    /* Decode     */
    vm_quad_t valA = vm->registers[rA];
    /* Execute    */
    vm_quad_t valE = valA;
    bool cnd = !zf(vm);
    /* Memory     */
    /* Write back */
    if (cnd) {
        vm->registers[rB] = valE;
    }
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  25 AB                           */
static inline void cmovge(VM* vm) {
    CERO_DEBUG("ins::cmovge\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valP = vm->pc + 2;
    /* Decode     */
    vm_quad_t valA = vm->registers[rA];
    /* Execute    */
    vm_quad_t valE = valA;
    bool cnd = !sf(vm);
    /* Memory     */
    /* Write back */
    if (cnd) {
        vm->registers[rB] = valE;
    }
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  80 ---------Dest----------      */
static inline void cmovg(VM* vm) {
    CERO_DEBUG("ins::cmovg\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_ubyte_t rB   = REG_SPEC_DEC_RB(rArB);
    vm_quad_t valP = vm->pc + 2;
    /* Decode     */
    vm_quad_t valA = vm->registers[rA];
    /* Execute    */
    vm_quad_t valE = valA;
    bool cnd = !sf(vm) & !zf(vm);
    /* Memory     */
    /* Write back */
    if (cnd) {
        vm->registers[rB] = valE;
    }
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  80 AB                           */
static inline void call(VM* vm) {
    CERO_DEBUG("ins::call\n");
    /* Fetch      */
    vm_quad_t valC = m8r(vm, vm->pc + 1);
    vm_quad_t valP = vm->pc + 9;
    /* Decode     */
    vm_quad_t valB = vm->registers[REG_RSP];
    /* Execute    */
    vm_quad_t valE = valB - 8;
    /* Memory     */
    if (addrInHighMem(vm, valE) || addrInHighMem(vm, valE + 8)) {
        vm->statusCondition = STAT_ADR;
        return;
    }
    m8w(vm, valE, valP);
    /* Write back */
    vm->registers[REG_RSP] = valE;
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  90                              */
static inline void ret(VM* vm) {
    CERO_DEBUG("ins::ret\n");
    /* Fetch      */
    vm_quad_t valP = vm->pc + 1;
    /* Decode     */
    vm_quad_t valA = vm->registers[REG_RSP];
    vm_quad_t valB = vm->registers[REG_RSP];
    /* Execute    */
    vm_quad_t valE = valB + 8;
    /* Memory     */
    vm_quad_t valM = m8r(vm, valA);
    /* Write back */
    vm->registers[REG_RSP] = valE;
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  20 AF                           */
static inline void pushq(VM* vm) {
    CERO_DEBUG("ins::pushq\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_quad_t valP = vm->pc + 2;
    /* Decode     */
    vm_quad_t valA = vm->registers[rA];
    vm_quad_t valB = vm->registers[REG_RSP];
    /* Execute    */
    vm_quad_t valE = valB - 8;
    /* Memory     */
    m8w(vm, valE, valA);
    /* Write back */
    vm->registers[REG_RSP] = valE;
    /* PC update  */
    vm->pc = valP;
}

/* 0  1  2  3  4  5  6  7  8  9  10 */
/*  20 AF                           */
static inline void popq(VM* vm) {
    CERO_DEBUG("ins::popq\n");
    /* Fetch      */
    vm_ubyte_t rArB = m1r(vm, vm->pc + 1);
    vm_ubyte_t rA   = REG_SPEC_DEC_RA(rArB);
    vm_quad_t valP = vm->pc + 2;
    /* Decode     */
    vm_quad_t valA = vm->registers[REG_RSP];
    vm_quad_t valB = vm->registers[REG_RSP];
    /* Execute    */
    vm_quad_t valE = valB + 8;
    /* Memory     */
    vm_quad_t valM = m8r(vm, valA);
    /* Write back */
    vm->registers[REG_RSP] = valE;
    vm->registers[rA] = valM;
    /* PC update  */
    vm->pc = valP;
}

void run(VM* vm) {
    while(vm->statusCondition == STAT_AOK) {
        switch ((vm_ubyte_t)m1r(vm, vm->pc)) {
            case INS_HALT:   halt(vm);              break;
            case INS_NOP:    nop(vm);               break;
            case INS_RRMOVQ: rrmovq(vm);            break;
            case INS_IRMOVQ: irmovq(vm);            break;
            case INS_RMMOVQ: rmmovq(vm);            break;
            case INS_MRMOVQ: mrmovq(vm);            break;
            case INS_ADDQ:   addq(vm);              break;
            case INS_SUBQ:   subq(vm);              break;
            case INS_ANDQ:   andq(vm);              break;
            case INS_XORQ:   xorq(vm);              break;
            case INS_JMP:    jmp(vm);               break;
            case INS_JLE:    jle(vm);               break;
            case INS_JL:     jl(vm);                break;
            case INS_JE:     je(vm);                break;
            case INS_JNE:    jne(vm);               break;
            case INS_JGE:    jge(vm);               break;
            case INS_JG:     jg(vm);                break;
            case INS_CMOVLE: cmovle(vm);            break;
            case INS_CMOVL:  cmovl(vm);             break;
            case INS_CMOVE:  cmove(vm);             break;
            case INS_CMOVNE: cmovne(vm);            break;
            case INS_CMOVGE: cmovge(vm);            break;
            case INS_CMOVG:  cmovg(vm);             break;
            case INS_CALL:   call(vm);              break;
            case INS_RET:    ret(vm);               break;
            case INS_PUSHQ:  pushq(vm);             break;
            case INS_POPQ:   popq(vm);              break;
            default: vm->conditionCodes = STAT_INS; break;
        }
        printMemory(vm);
        printRegisters(vm);
        printStack(vm);
        printFlagsAndStatusAndPc(vm);
    }
}
