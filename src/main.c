#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

/* int main(int argc, const char* argv[]) */
int main(int argc, const char* argv[]) {
    VM vm;
    initVM(&vm);
    Chunk chunk;
    initChunk(&chunk);
    irmovqWrite(&chunk, REG_R13, 123);
    rrmovqWrite(&chunk, REG_R13, REG_R10);
    /*writePushQ(&chunk, REG_R13);
    writeRrmovq(&chunk, (REG_R14 << 4) | REG_R10);
    writePushQ(&chunk, REG_R13);
    writePushQ(&chunk, REG_R13);
    writePopQ(&chunk,  REG_R14);
    writePushQ(&chunk, REG_R13);
    writePopQ(&chunk,  REG_R14);
    writePopQ(&chunk,  REG_R14);
    writePopQ(&chunk,  REG_R14);
    writeHalt(&chunk);*/
    run(&vm, &chunk);
    CERO_INFO("%i\n", vm.statusCondition);
    freeVM(&vm);
    return 0;
}