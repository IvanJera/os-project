#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

TCB* TCB::tail = nullptr;
TCB *TCB::running = nullptr;
TCB* TCB::head = nullptr;


uint64 TCB::timeSliceCounter = 0;
uint64 TCB::firstTimeLeftAsleep = 0;

int TCB::idCounter=0;

int TCB::getRunningId() {
    return running? running->id:-1;
}

int TCB::createThreadHandle(TCB** handle, Body body, void* arg, uint64* stackptr){
    *handle = new TCB(body, arg, DEFAULT_TIME_SLICE, stackptr);
    if(*handle) return 0;
    return -1;
}

void TCB::yield(){
    __asm__ volatile ("li a0, 0x13");
    //__asm__ volatile ("mv a0,%0"::"r" (THREAD_DISPATCH));
    __asm__ volatile ("ecall");
}

void TCB::dispatch() {
    TCB* old = running;
    if (!old->isFinished() && !old->isBlocked && !old->isSleeping) {
        Scheduler::put(old);
    } else {

        if (old->isFinished() && old->parent != nullptr) {
            old->parent->childCount--;
            if (old->parent->childCount == 0 && old->parent->childrenSem != nullptr) {
                old->parent->childrenSem->signal();
            }
        }

    }
    TCB* next = Scheduler::get();
    if (!next) return;
    running = next;
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    if(running->body) Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}

int TCB::exit() {
    running->setFinished(true);
    TCB::dispatch();
    return 0;
}

void TCB::block() {
    running->isBlocked = true;
    TCB::dispatch();
}

void TCB::unblock() {
    this->isBlocked = false;
    Scheduler::put(this);
}

int TCB::toSleep(time_t timeout) {
    if (TCB::running == nullptr) return -1;
    if (timeout == 0) return 0;

    TCB* me = TCB::running;
    me->isSleeping = true;

    if (head == nullptr) {
        me->thisTimeLeftAsleep = timeout;
        me->nextAsleep = nullptr;
        head = tail = me;
        firstTimeLeftAsleep = timeout;
        TCB::dispatch();
        return 0;
    }

    TCB* prev = nullptr;
    TCB* curr = head;
    uint64 acc = 0;

    while (curr != nullptr) {
        acc += curr->thisTimeLeftAsleep;
        if (timeout < acc) break;
        prev = curr;
        curr = curr->nextAsleep;
    }

    if (curr == nullptr) {
        me->thisTimeLeftAsleep = timeout - acc;
        me->nextAsleep = nullptr;
        tail->nextAsleep = me;
        tail = me;
        TCB::dispatch();
        return 0;
    }

    uint64 prevAbs = acc - curr->thisTimeLeftAsleep;
    me->thisTimeLeftAsleep = timeout - prevAbs;
    me->nextAsleep = curr;
    curr->thisTimeLeftAsleep = acc - timeout;

    if (prev != nullptr) {
        prev->nextAsleep = me;
    } else {
        head = me;
        firstTimeLeftAsleep = me->thisTimeLeftAsleep;
    }

    TCB::dispatch();
    return 0;
}

void TCB::wakeUp()
{
    TCB* readyHead = nullptr;
    TCB* readyTail = nullptr;

    for (;;)
    {
        if (head == nullptr || head->thisTimeLeftAsleep > 0)
            break;

        TCB* expired = head;
        head = head->nextAsleep;

        expired->nextAsleep = nullptr;
        expired->isSleeping = false;

        if (readyHead == nullptr)
        {
            readyHead = expired;
            readyTail = expired;
        }
        else
        {
            readyTail->nextAsleep = expired;
            readyTail = expired;
        }
    }

    if (head == nullptr)
        tail = nullptr;

    firstTimeLeftAsleep =
        head == nullptr ? 0 : head->thisTimeLeftAsleep;

    while (readyHead != nullptr)
    {
        TCB* ready = readyHead;
        readyHead = readyHead->nextAsleep;

        ready->nextAsleep = nullptr;
        Scheduler::put(ready);
    }
}

void TCB::updateAsleep()
{
    if (head != nullptr)
    {
        head->thisTimeLeftAsleep -= 1;
        firstTimeLeftAsleep = head->thisTimeLeftAsleep;
    }

    wakeUp();
}

void TCB::pullOutAsleepThread(TCB* thread)
{
    if (thread == nullptr)
        return;

    TCB** linkToCurrent = &head;
    TCB* previous = nullptr;

    while (*linkToCurrent != nullptr &&
           *linkToCurrent != thread)
    {
        previous = *linkToCurrent;
        linkToCurrent = &((*linkToCurrent)->nextAsleep);
    }

    if (*linkToCurrent == nullptr)
        return;

    TCB* removed = *linkToCurrent;
    TCB* following = removed->nextAsleep;

    if (following != nullptr)
    {
        following->thisTimeLeftAsleep +=
            removed->thisTimeLeftAsleep;
    }

    *linkToCurrent = following;

    if (removed == tail)
        tail = previous;

    removed->nextAsleep = nullptr;
    removed->isSleeping = false;

    firstTimeLeftAsleep =
        head == nullptr ? 0 : head->thisTimeLeftAsleep;
}

void TCB::addChild(TCB* parent, TCB* child) {
    if (parent->childrenSem == nullptr)
        sem::createSemaphore(&parent->childrenSem, 0);
    child->parent = parent;
    parent->childCount++;
    if (child->finished) {
        parent->childCount--;
        child->parent = nullptr;
    }
}

void TCB::joinAllChildren() {
    if (running->childCount <= 0) return;
    if (running->childrenSem == nullptr) {
        sem::createSemaphore(&running->childrenSem, 0);
    }
    running->childrenSem->wait();
}