#ifndef _SCHEDULER_HPP
#define _SCHEDULER_HPP

#include "../h/List.hpp"

class TCB;

class Scheduler{
public:
    static TCB *get();
    static void put(TCB *ccb);

private:
    static List<TCB> readyTCBQueue;
};

#endif
