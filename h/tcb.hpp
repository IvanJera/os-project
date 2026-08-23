#ifndef _TCB_HPP
#define _TCB_HPP

#include "../lib/hw.h"
#include "../h/Scheduler.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Sem.hpp"

class sem;

class TCB
{
public:
    using Body = void (*)(void*);

    static int createThreadHandle(TCB** handle, Body body, void* arg, uint64* stackptr);

    static TCB *running;

    static void yield();
    static void dispatch();
    static void block();
    void unblock();
    static int  exit();


    static void wakeUp();
    static int  toSleep(time_t timeout);

    static void updateAsleep();

    static void pullOutAsleepThread(TCB* t);



    bool   isFinished()   const { return finished; }
    void   setFinished(bool value) { finished = value; }

    static void* operator new(size_t size) {
        return MemoryAllocator::mem_alloc(size );
    }
    static void* operator new[](size_t size) {
        return MemoryAllocator::mem_alloc(size );
    }
    static void operator delete(void *ptr) {
        if (!ptr) return;
        MemoryAllocator::mem_free(ptr);
    }
    static void operator delete[](void *ptr) {
        if (!ptr) return;
        MemoryAllocator::mem_free(ptr);
    }
    uint64 getTimeSlice() const { return timeSlice; }
    ~TCB() {
        delete[] stack;
    }

    static void addChild(TCB* parent, TCB* child);
    static void joinAllChildren();

    int getId() const{return id;}
    static int getRunningId();


private:
    TCB(Body body, void* arg, uint64 timeSlice, uint64* stack) :
            body(body),
            arg(arg),
            stack(stack),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) stack + DEFAULT_STACK_SIZE : 0
                    }),
            timeSlice(timeSlice),
            finished(false),
            isBlocked(false),
            thisTimeLeftAsleep(0),
            nextAsleep(nullptr),
            isSleeping(false)//,
            parent(nullptr),
            childCount(0),
            childrenSem(nullptr),
            id(++idCounter)
    {
        if (body != nullptr) {
            Scheduler::put(this);
        }
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body   body;
    void*  arg;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    bool   finished;
    bool   isBlocked;

    uint64 thisTimeLeftAsleep;
    TCB*   nextAsleep;
    bool   isSleeping;

    TCB*   parent;
    int    childCount;
    sem*   childrenSem;

    int id;
    static int idCounter;

    unsigned semWaitN = 1;

    friend class Riscv;

    friend class sem;

    static void threadWrapper();
    static void contextSwitch(Context *oldContext, Context *runningContext);

    static TCB *head, *tail;

    static uint64 timeSliceCounter;
    static  uint64 firstTimeLeftAsleep;
    static uint64 systemTime;
};

#endif