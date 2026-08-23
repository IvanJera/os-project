# RISC-V Kernel

A small educational operating-system kernel for the RISC-V architecture, developed in C++ and RISC-V assembly as part of the Operating Systems 1 course at the School of Electrical Engineering, University of Belgrade.

The project implements the essential mechanisms required for running concurrent user threads: dynamic memory allocation, thread scheduling, context switching, synchronization, system calls, and interrupt handling.

## Features

- Preemptive multithreading and thread context switching
- Thread creation, termination, dispatching, and synchronization
- Scheduler with a queue of ready threads
- Counting semaphores with blocking and unblocking support
- System-call interface for threads, memory, semaphores, and console I/O
- Custom heap memory allocator
- RISC-V trap and interrupt handling
- Timer and external interrupt processing
- C and C++ APIs for interacting with the kernel
- Thread-safe circular buffer used for console communication

## Architecture

The implementation is divided into several logical layers:

- **Hardware layer** – RISC-V registers, privilege levels, traps, interrupts, and context switching
- **Kernel layer** – thread control blocks, scheduling, semaphores, and memory management
- **System-call layer** – controlled transition between user code and kernel services
- **API layer** – C and object-oriented C++ interfaces for threads and synchronization
- **Application layer** – user program and tests executed on top of the kernel

## Core Components

### Memory allocator

The kernel uses a custom allocator that manages the available heap as a linked list of free memory segments. Allocation requests are expressed in fixed-size memory blocks. Released adjacent segments are merged to reduce fragmentation.

### Threads and scheduling

Each thread is represented by a thread control block containing its execution context, stack, state, and entry function. Ready threads are placed in the scheduler queue, while blocked threads wait until the corresponding synchronization event occurs.

### Semaphores

Counting semaphores provide `wait` and `signal` operations. A thread that cannot continue is removed from the ready queue and placed in the semaphore's blocked-thread queue until another thread releases it.

### System calls

User-facing functions request kernel services through RISC-V environment calls. The trap handler decodes the requested operation, executes it in supervisor mode, and returns the result to the caller.

The interface includes operations for:

- Memory allocation and deallocation
- Thread management and dispatching
- Semaphore creation, waiting, signaling, and closing
- Console input and output

### Console buffer

Console communication uses a bounded circular buffer. Two semaphores track available items and free positions, allowing producer and consumer threads to exchange characters safely.

## Technologies

- C++
- RISC-V assembly
- GNU RISC-V toolchain
- QEMU RISC-V emulator
- Make

## Building and Running

### Requirements

Install the following tools before building the project:

- RISC-V GNU compiler toolchain
- QEMU with RISC-V 64-bit system emulation
- GNU Make

### Run

Clone the repository and enter the project directory:

```bash
git clone <repository-url>
cd <repository-directory>
```

Build and start the kernel in QEMU using the supplied Makefile:

```bash
make qemu
```

To remove generated build files:

```bash
make clean
```

## Example API

```cpp
#include "syscall_cpp.hpp"

class Worker : public Thread {
protected:
    void run() override {
        Console::putc('A');
    }
};

int userMain() {
    Worker worker;
    worker.start();
    Thread::dispatch();
    return 0;
}
```

## Learning Outcomes

This project demonstrates practical understanding of:

- Kernel and user execution modes
- Low-level context switching
- Thread life-cycle management
- Concurrent programming and synchronization
- Interrupt and exception processing
- Memory allocation without the standard runtime allocator
- Integration of C++, assembly, and hardware-specific interfaces

## Author

**Ivan Jerinić**  
Software Engineering, University of Belgrade – School of Electrical Engineering  
[GitHub](https://github.com/IvanJera)

## Disclaimer

This repository is an educational operating-systems project. It is not intended for production use.
