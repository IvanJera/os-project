#include "../h/Sem.hpp"
#include "../h/tcb.hpp"
#include "../h/Scheduler.hpp"

int sem::createSemaphore(sem **handle, unsigned int init) {
    *handle = new sem(init);
    return (*handle ? 0 : -1);
}

int sem::signal() {
    if (closed) return -1;
    val++;
    if (val <= 0) {
        TCB* t = blockedQueue.removeFirst();
        if (t) t->unblock();
    }
    return 0;
}
int sem::close() {
    if (closed) return -1;
    closed = true;
    while (blockedQueue.peekFirst()) {
        TCB* t = blockedQueue.removeFirst();
        if (t) {
            t->isBlocked = false;
            Scheduler::put(t);
        }
    }
    val = 0;
    return 0;
}

int sem::wait() {
    if(closed) return -1;
    val--;
    if((int)(val) < 0) {
        this->blockedQueue.addLast(TCB::running);
        TCB::running->isBlocked = true;
        TCB::dispatch();
        if(closed) return -1;
    }
    return 0;
}

int sem::waitN(unsigned n) {
    if (closed) return -1;
    if (val >= (int)n) {
        val -= n;
        return 0;
    }
    TCB::running->semWaitN = n;
    val -= n;
    blockedQueue.addLast(TCB::running);
    TCB::running->isBlocked = true;
    TCB::dispatch();
    if (closed) return -1;
    return 0;
}

int sem::signalN(unsigned n) {
    if (closed) return -1;
    val += n;
    while (blockedQueue.peekFirst() &&
           val >= (int)blockedQueue.peekFirst()->semWaitN) {
        TCB* t = blockedQueue.removeFirst();
        val -= t->semWaitN;
        t->unblock();
    }
    return 0;
}
