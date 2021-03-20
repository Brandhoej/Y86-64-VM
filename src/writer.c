#include "writer.h"
#include "vm.h"

void initWriter(Writer* writer, vm_byte_t* destination, uint32_t offset) {
    writer->offset      = offset;
    writer->destination = destination;
}

void freeWriter(Writer* writer) {
    initWriter(writer, NULL, 0);
}

static inline void writeBytes(Writer* writer, vm_ubyte_t* bytes, uint32_t length) {
    for (uint32_t i = 0; i < length; ++i) {
        writer->destination[writer->offset + i] = bytes[i];
    }
    writer->offset += length;
}

static inline void writeInsFun(Writer* writer, vm_ubyte_t insFun) {
    vm_ubyte_t bytes[1];
    bytes[0] = insFun;
    writeBytes(writer, bytes, 1);
}

static inline void writeInsFunRegs(Writer* writer, vm_ubyte_t insFun, vm_ubyte_t rArB) {
    vm_ubyte_t bytes[2];
    bytes[0] = insFun;
    bytes[1] = rArB;
    writeBytes(writer, bytes, 2);
}

static inline void writeInsFunRegsQuad(Writer* writer, vm_ubyte_t insFun, vm_ubyte_t rArB, vm_quad_t q) {
    vm_ubyte_t bytes[10];
    bytes[0] = insFun;
    bytes[1] = rArB;
    bytes[9] = (q >>  0) & 0xFF;
    bytes[8] = (q >>  8) & 0xFF;
    bytes[7] = (q >> 16) & 0xFF;
    bytes[6] = (q >> 24) & 0xFF;
    bytes[5] = (q >> 32) & 0xFF;
    bytes[4] = (q >> 40) & 0xFF;
    bytes[3] = (q >> 48) & 0xFF;
    bytes[2] = (q >> 56) & 0xFF;
    writeBytes(writer, bytes, 10);
}

static inline void writeInsFunQuad(Writer* writer, vm_ubyte_t insFun, vm_quad_t q) {
    vm_ubyte_t bytes[9];
    bytes[0] = insFun;
    bytes[8] = (q >>  0) & 0xFF;
    bytes[7] = (q >>  8) & 0xFF;
    bytes[6] = (q >> 16) & 0xFF;
    bytes[5] = (q >> 24) & 0xFF;
    bytes[4] = (q >> 32) & 0xFF;
    bytes[3] = (q >> 40) & 0xFF;
    bytes[2] = (q >> 48) & 0xFF;
    bytes[1] = (q >> 56) & 0xFF;
    writeBytes(writer, bytes, 9);
}

void haltWrite(Writer* writer) {
    writeInsFun(writer, INS_HALT);
}

void nopWrite(Writer* writer) {
    writeInsFun(writer, INS_NOP);
}

void rrmovqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB) {
    writeInsFunRegs(writer, INS_RRMOVQ, REG_SPEC_ENC_RARB(rA, rB));
}

void irmovqWrite(Writer* writer, vm_ubyte_t rB, vm_quad_t v) {
    writeInsFunRegsQuad(writer, INS_IRMOVQ, REG_SPEC_ENC_RB(rB), v);
}

void rmmovqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB, vm_quad_t d) {
    writeInsFunRegsQuad(writer, INS_RMMOVQ, REG_SPEC_ENC_RARB(rA, rB), d);
}

void mrmovqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB, vm_quad_t d) {
    writeInsFunRegsQuad(writer, INS_MRMOVQ, REG_SPEC_ENC_RARB(rA, rB), d);
}

void addqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB) {
    writeInsFunRegs(writer, INS_ADDQ, REG_SPEC_ENC_RARB(rA, rB));
}

void subqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB) {
    writeInsFunRegs(writer, INS_SUBQ, REG_SPEC_ENC_RARB(rA, rB));
}

void andqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB) {
    writeInsFunRegs(writer, INS_ANDQ, REG_SPEC_ENC_RARB(rA, rB));
}

void xorqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB) {
    writeInsFunRegs(writer, INS_XORQ, REG_SPEC_ENC_RARB(rA, rB));
}

void jmpWrite(Writer* writer, vm_quad_t dest) {
    writeInsFunQuad(writer, INS_JMP, dest);
}

void jleWrite(Writer* writer, vm_quad_t dest) {
    writeInsFunQuad(writer, INS_JLE, dest);
}

void jlWrite(Writer* writer, vm_quad_t dest) {
    writeInsFunQuad(writer, INS_JL, dest);
}

void jeWrite(Writer* writer, vm_quad_t dest) {
    writeInsFunQuad(writer, INS_JE, dest);
}

void jneWrite(Writer* writer, vm_quad_t dest) {
    writeInsFunQuad(writer, INS_JNE, dest);
}

void jgeWrite(Writer* writer, vm_quad_t dest) {
    writeInsFunQuad(writer, INS_JGE, dest);
}

void jgWrite(Writer* writer, vm_quad_t dest) {
    writeInsFunQuad(writer, INS_JG, dest);
}

void cmovleWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB) {
    writeInsFunRegs(writer, INS_CMOVLE, REG_SPEC_ENC_RARB(rA, rB));
}

void cmovlWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB) {
    writeInsFunRegs(writer, INS_CMOVL, REG_SPEC_ENC_RARB(rA, rB));
}

void cmoveWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB) {
    writeInsFunRegs(writer, INS_CMOVE, REG_SPEC_ENC_RARB(rA, rB));
}

void cmovneWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB) {
    writeInsFunRegs(writer, INS_CMOVNE, REG_SPEC_ENC_RARB(rA, rB));
}

void cmovgeWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB) {
    writeInsFunRegs(writer, INS_CMOVGE, REG_SPEC_ENC_RARB(rA, rB));
}

void cmovgWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB) {
    writeInsFunRegs(writer, INS_CMOVG, REG_SPEC_ENC_RARB(rA, rB));
}

void callWrite(Writer* writer, vm_quad_t dest) {
    writeInsFunQuad(writer, INS_CALL, dest);
}

void retWrite(Writer* writer) {
    writeInsFun(writer, INS_RET);
}

void pushqWrite(Writer* writer, vm_ubyte_t rA) {
    writeInsFunRegs(writer, INS_PUSHQ, REG_SPEC_ENC_RA(rA));
}

void popqWrite(Writer* writer, vm_ubyte_t rA) {
    writeInsFunRegs(writer, INS_POPQ, REG_SPEC_ENC_RA(rA));
}
