#ifndef _SYSCALL_C_H
#define _SYSCALL_C_H

#include "../lib/hw.h"

enum syscalls {
    MEM_ALLOC = 0x01, MEM_FREE,
    THREAD_CREATE = 0x11, THREAD_EXIT, THREAD_DISPATCH, THREAD_ADD_CHILD,THREAD_JOIN_ALL,THREAD_GET_ID,
    SEM_OPEN = 0x21, SEM_CLOSE, SEM_WAIT, SEM_SIGNAL,SEM_WAIT_N,SEM_SIGNAL_N,
    TIME_SLEEP = 0x31,
    GETC = 0x41, PUTC
};

void* mem_alloc(size_t size);
int mem_free(void*);

class TCB;
typedef TCB* thread_t;

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg);
int thread_exit();
void thread_dispatch();
int thread_add_child(thread_t child);
int thread_join_all();
int getThreadId();

class sem;
typedef sem* sem_t;

int sem_open(sem_t* handle, unsigned init);
int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);

int sem_wait_n(sem_t id, unsigned n);
int sem_signal_n(sem_t id, unsigned n);

typedef unsigned long time_t;
int time_sleep(time_t t);

const int EOF = -1;

char getc();
void putc(char);


#endif
