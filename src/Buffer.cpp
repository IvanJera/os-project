#include "../h/Buffer.hpp"
#include "../h/syscall_c.hpp"
#include "../h/MemoryAllocator.hpp"

_Buffer::_Buffer(int _cap) : capacity(_cap + 1), head(0), tail(0) {
    buffer = (char *) MemoryAllocator::mem_alloc(sizeof(char) * capacity / MEM_BLOCK_SIZE + 1);
    sem_open(&itemAvailable, 0);
    sem_open(&spaceAvailable, _cap);
}

_Buffer::~_Buffer() {
    MemoryAllocator::mem_free(buffer);
    sem_close(itemAvailable);
    sem_close(spaceAvailable);

}

void _Buffer::put(char val) {
    sem_wait(spaceAvailable);
    buffer[tail] = val;
    tail = (tail + 1) % capacity;

    sem_signal(itemAvailable);
}

char _Buffer::get() {
    sem_wait(itemAvailable);

    char ret = buffer[head];
    head = (head + 1) % capacity;

    sem_signal(spaceAvailable);
    return ret;
}

int _Buffer::getCnt() {
    int ret;

    if (tail >= head) {
        ret = tail - head;
    } else {
        ret = capacity - head + tail;
    }
    return ret;
}

int _Buffer::getCapacity(){
    return capacity;
}
