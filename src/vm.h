#ifndef creo_vm_h
#define creo_vm_h

#include "chunk.h"
#include "value.h"

#define STACK_MAX  (256)
#define REG_COUNT  (16)

#define REG_RAX    (0x0) /*                                                    */
#define REG_RCX    (0x1) /*                                                    */
#define REG_RDX    (0x2) /*                                                    */
#define REG_RRBX   (0x3) /*                                                    */
#define REG_RSP    (0x4) /*                                                    */
#define REG_RBP    (0x5) /* The base pointer of the current stack (RBP <= RSP) */
#define REG_RSI    (0x6) /* The top pointer of the current stack  (RSP >= RBP) */
#define REG_RDI    (0x7) /*                                                    */
#define REG_R8     (0x8) /*                                                    */
#define REG_R9     (0x9) /*                                                    */
#define REG_R10    (0xA) /*                                                    */
#define REG_R11    (0xB) /*                                                    */
#define REG_R12    (0xC) /*                                                    */
#define REG_R13    (0xD) /*                                                    */
#define REG_R14    (0xE) /*                                                    */
#define REG_F      (0xF) /*                                                    */

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
#define INS_CMOVLE (0x71)
#define INS_CMOVL  (0x72)
#define INS_CMOVE  (0x73)
#define INS_CMOVNE (0x74)
#define INS_CMOVGE (0x75)
#define INS_CMOVG  (0x76)
#define INS_CALL   (0x80)
#define INS_RET    (0x90)
#define INS_PUSHQ  (0xA0)
#define INS_POPQ   (0xB0)

#define CC_ZF      (0x00)
#define CC_SF      (0x01)
#define CC_OF      (0x02)

typedef enum {
    STAT_AOK, /* Normal operation                                      */
    STAT_HLT, /* Halt instruciton  encountered                         */
    STAT_ADR, /* Bad address (either instruction or data)  encountered */
    STAT_INS  /* Invalid  instruction encountered                      */
} StatusCondition;

typedef struct {
    Chunk* chunk;                    /* The current chunk used by the VM                                            */
    ByteWord* pc;                    /* The Program Counter pointing at the current isntruction in the chunk opCode */
    StatusCondition statusCondition; /* The status of the VM                                                        */
    ByteWord conditionCodes;         /* Byte container for the CC_ZF, CC_SF and CC_OF                               */
    QuadWord registers[REG_COUNT];   /* The 16 registers used by the y86-64 mapped with REG_X                       */
    QuadWord stack[STACK_MAX];       /* The virtual memory stack used by this VM                                    */
} VM;

void initVM(VM* vm);
void freeVM(VM* vm);

void run(VM* vm, Chunk* chunk);

#endif