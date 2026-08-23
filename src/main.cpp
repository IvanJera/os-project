#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/console.hpp"

extern void userMain();

static void halt_qemu() {
    volatile unsigned int* f = (volatile unsigned int*)0x100000;
    *f = 0x5555;
    while (1) { }
}

void wrapperMain(void* args) {
    userMain();
}

int main(){
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    TCB *userThread;
    TCB* mainThread;
    thread_create(&mainThread, nullptr, nullptr);
    thread_create(&userThread, wrapperMain, nullptr);

    TCB::running = mainThread;

    console::init();
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while(!userThread->isFinished()) {
        thread_dispatch();
    }

    __asm__ volatile ("li t3, 0x100000");
    __asm__ volatile ("li t4, 0x5555");
    __asm__ volatile ("sw t4, 0(t3)");
    return 0;
}