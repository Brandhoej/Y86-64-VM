#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "writer.h"
#include "common.h"
#include "vm.h"

int main(int argc, const char* argv[]) {
    VM vm;
    initVM(&vm);
    Writer writer;
    initWriter(&writer, vm.memory, 0);

    irmovqWrite(&writer, REG_R10, 0);
    irmovqWrite(&writer, REG_R11, 0);
    addqWrite(&writer, REG_R10, REG_R11);
    jneWrite(&writer, writer.offset + 1); // Skips nop if true
    nopWrite(&writer);
    haltWrite(&writer);

    run(&vm);
    freeVM(&vm);
    return 0;
}
