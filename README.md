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


### System calls

User-facing functions request kernel services through RISC-V environment calls. The trap handler decodes the requested operation, executes it in supervisor mode, and returns the result to the caller.

The interface includes operations for:

- Memory allocation and deallocation
- Thread management and dispatching
- Semaphore creation, waiting, signaling, and closing
- Console input and output


## Technologies

- C++
- RISC-V assembly
- GNU RISC-V toolchain
- QEMU RISC-V emulator
- Make
