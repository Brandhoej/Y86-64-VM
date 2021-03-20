#ifndef cero_printer_h
#define cero_printer_h

#include "common.h"
#include "vm.h"

#define DEBUG_TRACE_EXECUTION

#include "printer.h"

void printRegisters(VM* vm);
void printFlagsAndStatusAndPc(VM* vm);
void printStack(VM* vm);
void printMemory(VM* vm);

#endif