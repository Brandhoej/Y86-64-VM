#include "printer.h"

void printRegisters(VM* vm) {
    const vm_quad_t columnCount = 3;
    const vm_quad_t rowCount    = REG_COUNT / columnCount + 1;
    const vm_quad_t offset      = rowCount * columnCount - REG_COUNT;
    for (vm_quad_t r = 0; r < rowCount; ++r) {
        CERO_TRACE();
        for (vm_quad_t c = 0; c < columnCount; ++c) {
            vm_quad_t offsetR = rowCount - r - 1;
            vm_quad_t offsetC = columnCount - c - 1;
            vm_quad_t i       = (REG_COUNT - 1) - (offsetR + offsetC * rowCount);
            if (i < 0 || i >= REG_COUNT) {
                CERO_PRINT("                                    ");
                continue;
            }
            switch (i) {
                case  0: CERO_PRINT("RAX     "); break;
                case  1: CERO_PRINT("RCX     "); break;
                case  2: CERO_PRINT("RDX     "); break;
                case  3: CERO_PRINT("RBX     "); break;
                case  4: CERO_PRINT("RSP     "); break;
                case  5: CERO_PRINT("RBP     "); break;
                case  6: CERO_PRINT("RSI     "); break;
                case  7: CERO_PRINT("RDI     "); break;
                case  8: CERO_PRINT("R8      "); break;
                case  9: CERO_PRINT("R9      "); break;
                case 10: CERO_PRINT("R10     "); break;
                case 11: CERO_PRINT("R11     "); break;
                case 12: CERO_PRINT("R12     "); break;
                case 13: CERO_PRINT("R13     "); break;
                case 14: CERO_PRINT("R14     "); break;
                case 15: CERO_PRINT("F       "); break;
            }
            CERO_PRINT("0x%016" PRIx64, vm->registers[i]);
            CERO_PRINT("          ");
        }
        CERO_PRINT("\n");
    }
}

void printFlagsAndStatusAndPc(VM* vm) {
    CERO_TRACE();
    CERO_PRINT("ZF:%d   ", (vm->conditionCodes & 0b0001) >> CC_ZF);
    CERO_PRINT("SF:%d   ", (vm->conditionCodes & 0b0010) >> CC_SF);
    CERO_PRINT("OF:%d   ", (vm->conditionCodes & 0b0100) >> CC_OF);
    CERO_PRINT("STAT:");
    switch (vm->statusCondition) {
        case STAT_AOK: CERO_PRINT("AOK"); break;
        case STAT_HLT: CERO_PRINT("HLT"); break;
        case STAT_ADR: CERO_PRINT("ADR"); break;
        case STAT_INS: CERO_PRINT("INS"); break;
    }
    CERO_PRINT("   ");
    CERO_PRINT("PC:0x%06" PRIxPTR, vm->pc);
    CERO_PRINT("\n");
}

void printStack(VM* vm) {
    vm_quad_t rbp = vm->registers[REG_RBP];
    vm_quad_t rsp = vm->registers[REG_RSP];
    if (rbp == rsp) {
        CERO_TRACE("[ ]");
    } else {
        CERO_TRACE();
        for (vm_quad_t mem = rsp; mem < rbp; mem += 8) {
            CERO_PRINT("[ %" PRId64 " ]", m8r(vm, mem));
        }
    }
    CERO_PRINT("\n");
}

void printMemory(VM* vm) {
    vm_quad_t rbp = vm->registers[REG_RBP];
    vm_quad_t rsp = vm->registers[REG_RSP];
    const vm_quad_t memorySize     = MEM_MAX;
    const vm_quad_t bytesPerItem   = 8;
    const vm_quad_t columnCount    = 3;
    const vm_quad_t bytesPerColumn = columnCount * bytesPerItem;
    const vm_quad_t rowCount       = (memorySize + bytesPerColumn) / bytesPerColumn;
    for (vm_quad_t row = 0; row < rowCount; ++row) {
        CERO_TRACE();
        for (vm_quad_t column = 0; column < bytesPerColumn; ++column) {
            vm_quad_t iRow    = rowCount - row - 1;
            vm_quad_t iColumn = bytesPerColumn - column - 1;
            vm_quad_t iByte   = ((rowCount * bytesPerItem * (iColumn / bytesPerItem)) + (iRow * bytesPerItem) + (bytesPerItem - 1) - ((iRow * bytesPerItem) + iColumn) % bytesPerItem);
            if (iColumn % bytesPerItem == bytesPerItem - 1) {
                CERO_PRINT("0x%04" PRIx64, iByte);
                CERO_PRINT("  0x");
            }
            if (iByte >= 0 && iByte < memorySize) {
                CERO_PRINT("%02" PRIx8, vm->memory[iByte]);
            } else {
                CERO_PRINT("__");
            }
            if (iColumn > 0 && iColumn % 8 == 0) {
                bool printRBP = false, printRSP = false;
                if (rsp > iByte - 8 && rsp <= iByte) {
                    printRSP = true;
                }
                if(rbp > iByte - 8 && rbp <= iByte) {
                    printRBP = true;
                }

                if (printRBP && printRSP) {
                    CERO_PRINT("<RBP RSP  ");
                } else if (printRBP) {
                    CERO_PRINT("<RBP      ");
                }  else if (printRSP) {
                    CERO_PRINT("<RSP      ");
                } else {
                    CERO_PRINT("          ");
                }
            }
        }
        CERO_PRINT("\n");
    }
}