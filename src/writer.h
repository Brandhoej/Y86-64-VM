#ifndef cero_writer_h
#define cero_writer_h

#include "common.h"
#include "value.h"

/* Encodings                                            */
#define REG_SPEC_ENC_RARB(rA, rB) ((rA << 4) | rB)        
#define REG_SPEC_ENC_RA(ra) (REG_SPEC_ENC_RARB(ra, REG_F))
#define REG_SPEC_ENC_RB(rb) (REG_SPEC_ENC_RARB(REG_F, rb))
/* Decodings                                            */
#define REG_SPEC_DEC_RA(regSpec) (regSpec >> 4)  
#define REG_SPEC_DEC_RB(regSpec) (regSpec & 0x0F)

typedef struct {
    uint32_t offset;
    vm_byte_t* destination;
} Writer;

void initWriter(Writer* writer, vm_byte_t* destination, uint32_t offset);
void freeWriter(Writer* writer);

/* Execution  */
void haltWrite(Writer* writer);
void nopWrite(Writer* writer);
/* Register   */
void rrmovqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB);
void irmovqWrite(Writer* writer, vm_ubyte_t rB, vm_quad_t v);
void rmmovqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB, vm_quad_t d);
void mrmovqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB, vm_quad_t d);
/* Operations */
void addqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB);
void subqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB);
void andqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB);
void xorqWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB);
/* Branches   */
void jmpWrite(Writer* writer, vm_quad_t dest);
void jleWrite(Writer* writer, vm_quad_t dest);
void jlWrite(Writer* writer,  vm_quad_t dest);
void jeWrite(Writer* writer,  vm_quad_t dest);
void jneWrite(Writer* writer, vm_quad_t dest);
void jgeWrite(Writer* writer, vm_quad_t dest);
void jgWrite(Writer* writer,  vm_quad_t dest);
/* Moves      */
void cmovleWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB);
void cmovlWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB);
void cmoveWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB);
void cmovneWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB);
void cmovgeWrite(Writer* writer, vm_ubyte_t rA, vm_ubyte_t rB);
void cmovgWrite(Writer* writer,  vm_ubyte_t rA, vm_ubyte_t rB);
/* Computation */
void callWrite(Writer* writer, vm_quad_t dest);
void retWrite(Writer* writer);
/* Stack      */
void pushqWrite(Writer* writer, vm_ubyte_t rA);
void popqWrite(Writer* writer,  vm_ubyte_t rA);

#endif