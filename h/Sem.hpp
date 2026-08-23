#ifndef _SEM_HPP
#define _SEM_HPP

#include "../lib/hw.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/List.hpp"

class TCB;

class sem {
public:
    sem(int init = 1) : val(init), closed(false) {}
    int signal();
    int close();
    int wait();

    int waitN(unsigned n);
    
    int signalN(unsigned n);

    static int createSemaphore(sem** handle, unsigned init);

    static void* operator new(size_t size) {
        return MemoryAllocator::mem_alloc(size);
    }
    static void* operator new[](size_t size) {
        return MemoryAllocator::mem_alloc(size);
    }

    static void operator delete(void *ptr) {
        MemoryAllocator::mem_free(ptr);
    }
    static void operator delete[](void *ptr) {
        MemoryAllocator::mem_free(ptr);
    }
private:
    List<TCB> blockedQueue;
    int val;
    bool closed;
};

#endif
