#include "../h/Scheduler.hpp"

List<TCB> Scheduler::readyTCBQueue;

void Scheduler::put(TCB *a){
    readyTCBQueue.addLast(a);
}

TCB *Scheduler::get(){
    return readyTCBQueue.removeFirst();
}