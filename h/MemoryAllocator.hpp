#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {
public:
    static void* mem_alloc(size_t blocks);
    static int   mem_free(void* ptr);

    static void* kmalloc(size_t blocks);
    static int   kfree(void* ptr);

private:
    struct Chunk {
        size_t bytes;
        Chunk* link;
    };

    static Chunk* freeList;
    static bool   ready;

    static void   setup();
    static bool   adjacent(Chunk* a, Chunk* b);
};

#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP