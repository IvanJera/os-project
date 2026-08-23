#include "../h/MemoryAllocator.hpp"

MemoryAllocator::Chunk* MemoryAllocator::freeList = nullptr;
bool MemoryAllocator::ready = false;

void MemoryAllocator::setup() {
    freeList = (Chunk*) HEAP_START_ADDR;
    freeList->bytes = (size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR;
    freeList->link  = nullptr;
    ready = true;
}

bool MemoryAllocator::adjacent(Chunk* a, Chunk* b) {
    return a && b && ((char*)a + a->bytes == (char*)b);
}

void* MemoryAllocator::mem_alloc(size_t blocks) {
    if (!ready) setup();
    if (blocks == 0) return nullptr;

    size_t want = blocks * MEM_BLOCK_SIZE + sizeof(Chunk);
    want = ((want + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE;

    Chunk* walker = freeList;
    Chunk* behind = nullptr;

    for (; walker; behind = walker, walker = walker->link) {
        if (walker->bytes >= want) break;
    }
    if (!walker) return nullptr;

    size_t leftover = walker->bytes - want;

    if (leftover >= sizeof(Chunk) + MEM_BLOCK_SIZE) {
        Chunk* tail = (Chunk*)((char*)walker + want);
        tail->bytes = leftover;
        tail->link  = walker->link;

        if (behind) behind->link = tail;
        else freeList = tail;

        walker->bytes = want;
    } else {
        if (behind) behind->link = walker->link;
        else freeList = walker->link;
    }

    return (void*)((char*)walker + sizeof(Chunk));
}

void* MemoryAllocator::kmalloc(size_t blocks) {
    return mem_alloc(blocks);
}

int MemoryAllocator::kfree(void* ptr) {
    return mem_free(ptr);
}

int MemoryAllocator::mem_free(void* ptr) {
    if (!ptr) return -1;
    if ((char*)ptr < (char*)HEAP_START_ADDR ||
        (char*)ptr >= (char*)HEAP_END_ADDR) return -2;

    Chunk* freed = (Chunk*)((char*)ptr - sizeof(Chunk));

    Chunk* walker = freeList;
    Chunk* behind = nullptr;
    while (walker && walker < freed) {
        behind = walker;
        walker = walker->link;
    }

    freed->link = walker;
    if (behind) behind->link = freed;
    else freeList = freed;

    if (adjacent(freed, freed->link)) {
        freed->bytes += freed->link->bytes;
        freed->link = freed->link->link;
    }

    if (adjacent(behind, freed)) {
        behind->bytes += freed->bytes;
        behind->link = freed->link;
    }

    return 0;
}

