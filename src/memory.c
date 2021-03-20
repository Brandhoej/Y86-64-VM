#include <stdlib.h>
#include "memory.h"

/* Operation to perform based on the actual parameters: 
 * /---------------------------------------------------------------\
 * | oldSize  | newSize              | Operation                   |
 * |----------+----------------------+-----------------------------|
 * | 0        | Non-zero             | Allocate new block.         |
 * | Non-zero | 0                    | Free allocation.            |
 * | Non-zero | Smaller than oldSize | Shrink existing allocation. |
 * | Non-zero | Greater than oldSize | Grow existing allocation.   |
 * \---------------------------------------------------------------/
 * Handles allocating memoty, freeing it and changing the size of an
 * existing allocation. Routing all of those operations through a single function
 * is done for the convenience of the garbage collector (GC) and statistics.
 * */
void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }

    /* realloc handles cases: 1, 3, 4.
     * 
     * If the new size is smaller than the existing block of memory,
     * it simply updates the size of the block and returns the same pointer you gave it.
     * If the new size is larger, it attempts to grow the existing block of memory.
     * 
     * It can do that only if the memory after that block isn’t already in use.
     * If there isn’t room to grow the block, realloc() instead allocates a new 
     * block of memory of the desired size, copies over the old bytes,
     * frees the old block, and then returns a pointer to the new block. */
    void* result = realloc(pointer, newSize);
    if (result == NULL) exit(1);
    return result;
}