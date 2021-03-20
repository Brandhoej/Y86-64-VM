#ifndef creo_vm_h
#define creo_vm_h

#include "value.h"

/* This is the memory for the VM */
#define MEM_MAX    (256)

/* How to reserve memory: Expand the */

/* Conceptually, the stack is divided into two areas:
 * high addresses are all in use and reserved (you can't change these values!),       
 * and lower addresses that are unused (free or scratch space).                       
 * The stack pointer points to the last in-use byte of the stack.                     
 * The standard convention is that when your function starts up,                      
 * you can claim some of the stack by moving the stack pointer down--this indicates   
 * to any functions you might call that you're using those bytes of the stack.        
 * You can then use that memory for anything you want,                                
 * as long as you move the stack pointer back before your function returns.         */

/* (0) -> Caller-save   
 * (X) -> Callee-save   
 * (A) -> Args        */
#define REG_RAX    (0x00) /* ( O)                                                    */
#define REG_RCX    (0x01) /* (AO)                                                    */
#define REG_RDX    (0x02) /* (AO)                                                    */
#define REG_RBX    (0x03) /* ( X)                                                    */
#define REG_RSP    (0x04) /* (  ) The top pointer of the current stack  (RSP >= RBP) */
#define REG_RBP    (0x05) /* ( X) The base pointer of the current stack (RBP <= RSP) */
#define REG_RSI    (0x06) /* (AO)                                                    */
#define REG_RDI    (0x07) /* (AO)                                                    */
#define REG_R8     (0x08) /* (AO)                                                    */
#define REG_R9     (0x09) /* (AO)                                                    */
#define REG_R10    (0x0A) /* ( O)                                                    */
#define REG_R11    (0x0B) /* ( O)                                                    */
#define REG_R12    (0x0C) /* ( X)                                                    */
#define REG_R13    (0x0D) /* ( X)                                                    */
#define REG_R14    (0x0E) /* ( X)                                                    */
#define REG_F      (0x0F) /* (  )                                                    */
#define REG_COUNT  (16)

/* icode:ifun <- M1[PC] */
#define INS_HALT   (0x00)
#define INS_NOP    (0x10)
#define INS_RRMOVQ (0x20)
#define INS_IRMOVQ (0x30)
#define INS_RMMOVQ (0x40)
#define INS_MRMOVQ (0x50)
#define INS_ADDQ   (0x60)
#define INS_SUBQ   (0x61)
#define INS_ANDQ   (0x62)
#define INS_XORQ   (0x63)
#define INS_JMP    (0x70)
#define INS_JLE    (0x71)
#define INS_JL     (0x72)
#define INS_JE     (0x73)
#define INS_JNE    (0x74)
#define INS_JGE    (0x75)
#define INS_JG     (0x76)
#define INS_CMOVLE (0x21)
#define INS_CMOVL  (0x22)
#define INS_CMOVE  (0x23)
#define INS_CMOVNE (0x24)
#define INS_CMOVGE (0x25)
#define INS_CMOVG  (0x26)
#define INS_CALL   (0x80)
#define INS_RET    (0x90)
#define INS_PUSHQ  (0xA0)
#define INS_POPQ   (0xB0)

/* Set by arithmetic and logical operations */
#define CC_ZF      (0x00) /* Zero           */
#define CC_SF      (0x01) /* Negative sign  */
#define CC_OF      (0x02) /* Overflow       */

typedef enum {
    STAT_AOK, /* Normal operation                                      */
    STAT_HLT, /* Halt instruciton  encountered                         */
    STAT_ADR, /* Bad address (either instruction or data)  encountered */
    STAT_INS  /* Invalid  instruction encountered                      */
} StatusCondition;

typedef struct {
    vm_quad_t pc;                    /* The Program Counter pointing at the current isntruction in the chunk opCode */
    StatusCondition statusCondition; /* The status of the VM                                                        */
    vm_ubyte_t conditionCodes;       /* Byte container for the CC_ZF, CC_SF and CC_OF                               */
    vm_quad_t* registers;            /* The 16 registers used by the y86-64 mapped with REG_X                       */
    vm_ubyte_t* memory;              /* The virtual memory stack used by this VM                                    */
} VM;

void initVM(VM* vm);
void freeVM(VM* vm);

bool zf(VM* vm);
bool sf(VM* vm);
bool of(VM* vm);

vm_ubyte_t m1r(VM* vm, vm_quad_t offset);
vm_quad_t m8r(VM* vm, vm_quad_t offset);
vm_ubyte_t m1w(VM* vm, vm_quad_t offset, vm_ubyte_t byte);
vm_quad_t m8w(VM* vm, vm_quad_t offset, vm_quad_t quad);

void run(VM* vm);

#endif