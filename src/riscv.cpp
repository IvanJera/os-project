#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Sem.hpp"
#include "../h/console.hpp"
#include "../test/printing.hpp"

void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 volatile scause = r_scause();
    uint64 volatile a0 = Riscv::r_reg(REG_A0);
    /*uint64 volatile a1 = Riscv::r_a1();
    uint64 volatile a2 = Riscv::r_a2();
    uint64 volatile a3 = Riscv::r_a3();
    uint64 volatile a4 = Riscv::r_a4();*/
    if (scause == ECALL_SUPERVISOR || scause == ECALL_USER)
    {
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        int volatile retval;
        char volatile c;

        switch (a0) {
            case MEM_ALLOC:
                void* volatile pointer;
                pointer = MemoryAllocator::mem_alloc((size_t) (uint64)r_reg(REG_A1));
                __asm__ volatile ("sd %0, 80(s0)" : : "r"(pointer));
                break;

            case MEM_FREE:
                retval = MemoryAllocator::mem_free((void*) (uint64)r_reg(REG_A1));
                __asm__ volatile ("sd %0, 80(s0)" : : "r"(retval));
                break;

            case THREAD_CREATE:
                retval = TCB::createThreadHandle((TCB**) (uint64)r_reg(REG_A1), (void(*)(void*)) (uint64)r_reg(REG_A2), (void*) (uint64)r_reg(REG_A3), (uint64*) (uint64)r_reg(REG_A4));
                __asm__ volatile ("sd %0, 80(s0)" : : "r"(retval));
                break;

            case THREAD_EXIT:
                TCB::exit();
                __asm__ volatile ("li t1, 0x0");
                __asm__ volatile ("sw t1, 80(s0)");
                break;

            case THREAD_DISPATCH:
                TCB::dispatch();
                break;
            case THREAD_GET_ID:
                retval=TCB::getRunningId();
                __asm__ volatile ("sd %0, 80(s0)" : : "r"(retval));
                TCB::dispatch();
                break;
            case SEM_OPEN:
                retval = sem::createSemaphore((sem**) (uint64 volatile)r_reg(REG_A1), (unsigned) (uint64 volatile)r_reg(REG_A2));
                __asm__ volatile ("sd %0, 80(s0)" : : "r"(retval));
                break;

            case SEM_CLOSE:
                retval = ((sem*)(uint64) r_reg(REG_A1))->close();
                __asm__ volatile ("sd %0, 80(s0)" : : "r"(retval));
                break;

            case SEM_WAIT:
                retval = ((sem*)(uint64) r_reg(REG_A1))->wait();
                __asm__ volatile ("sd %0, 80(s0)" : : "r"(retval));
                break;

            case SEM_SIGNAL:
                retval = ((sem*)(uint64 volatile) r_reg(REG_A1))->signal();
                __asm__ volatile ("sd %0, 80(s0)" : : "r"(retval));
                break;

            case SEM_WAIT_N: {
                retval = ((sem*)(uint64)r_reg(REG_A1))->waitN((unsigned)r_reg(REG_A2));
                __asm__ volatile("sd %0, 80(s0)" : : "r"(retval));
                break;
            }
            case SEM_SIGNAL_N: {
                retval = ((sem*)(uint64)r_reg(REG_A1))->signalN((unsigned)r_reg(REG_A2));
                __asm__ volatile("sd %0, 80(s0)" : : "r"(retval));
                break;
            }
            case TIME_SLEEP:
                //time_sleep()
                retval = TCB::toSleep((uint64 volatile) r_reg(REG_A1));
                __asm__ volatile ("sd %0, 80(s0)" : : "r"(retval));
                break;

            case GETC:
                c = console::getc();
                __asm__ volatile ("sd %0, 80(s0)" : : "r"(c));
                break;

            case PUTC:
                console::putc((char)(uint64)r_reg(REG_A1));
                break;
            case THREAD_ADD_CHILD: {
                TCB* child = (TCB*)(uint64)r_reg(REG_A1);
                if (!child->finished) {
                    child->parent = TCB::running;
                    TCB::running->childCount++;
                    if (TCB::running->childrenSem == nullptr) {
                        sem::createSemaphore(&TCB::running->childrenSem, 0);
                    }
                }
                __asm__ volatile("li t1, 0x0");
                __asm__ volatile("sw t1, 80(s0)");
                break;
            }
            case THREAD_JOIN_ALL:{
                TCB::joinAllChildren();
                __asm__ volatile ("li t1, 0x0");
                __asm__ volatile ("sw t1, 80(s0)");
                break;
            }
            default:
                break;
        }

        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == TIMER_INTERRUPT)
    {
        mc_sip(SIP_SSIE);
        TCB::updateAsleep();
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            uint64 volatile sepc = r_sepc();
            uint64 volatile sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
    }
    else if (scause == CONSOLE_INTERRUPT) {
        uint64 sepc = r_sepc();
        uint64 sstatus = r_sstatus();

        uint64 ir_num = plic_claim();
        if (ir_num == CONSOLE_IRQ) {
            console::rxFill();
            console::txDrain();
        }
        if (ir_num) plic_complete((int)ir_num);

        w_sepc(sepc);
        w_sstatus(sstatus);
    }

    else{
        w_sepc(r_sepc() + 4);

    }
}