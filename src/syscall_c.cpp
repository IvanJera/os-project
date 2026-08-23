#include"../h/syscall_c.hpp"

void* mem_alloc(size_t size) {
    if(size == 0) return nullptr;

    size_t blocks = ((size + MEM_BLOCK_SIZE - 1)) / MEM_BLOCK_SIZE;

    __asm__ volatile("mv a1, %[blocks]" : : [blocks] "r"(blocks));
    __asm__ volatile("mv a0, %0" : : "r"(MEM_ALLOC));

    __asm__ volatile("ecall");

    void* volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int mem_free(void* ptr) {
    if(ptr == nullptr) return -1;

    __asm__ volatile("mv a1, %[k]" : : [k] "r"(ptr));
    __asm__ volatile("mv a0, %0" : : "r"(MEM_FREE));

    __asm__ volatile("ecall");

    size_t volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int sem_wait_n(sem_t id, unsigned n) {
    __asm__ volatile("mv a2, %0" : : "r"(n));
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("mv a0, %0" : : "r"(SEM_WAIT_N));
    __asm__ volatile("ecall");
    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int sem_signal_n(sem_t id, unsigned n) {
    __asm__ volatile("mv a2, %0" : : "r"(n));
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("mv a0, %0" : : "r"(SEM_SIGNAL_N));
    __asm__ volatile("ecall");
    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int thread_create(thread_t* handle, void (*body)(void*), void* arg) {
    void* stack = mem_alloc(DEFAULT_STACK_SIZE);
    if(stack == nullptr) return -1;

    __asm__ volatile("mv a4, %[stack]" : : [stack] "r"((uint64)stack));
    __asm__ volatile("mv a3, %[arg]" : : [arg] "r"(arg));
    __asm__ volatile("mv a2, %[body]" : : [body] "r" (body));
    __asm__ volatile("mv a1, %[handle]" : : [handle] "r"(handle));
    __asm__ volatile("mv a0, %0" : : "r"(THREAD_CREATE));

    __asm__ volatile("ecall");

    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int thread_exit() {
    __asm__ volatile("mv a0, %0" : : "r"(THREAD_EXIT));
    __asm__ volatile("ecall");

    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

void thread_dispatch() {
    __asm__ volatile("mv a0, %0" : : "r"(THREAD_DISPATCH));
    __asm__ volatile("ecall");
}

int sem_open(sem_t* handle, unsigned init){
    __asm__ volatile("mv a2, %[init]" : : [init] "r"(init));
    __asm__ volatile("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile("mv a0, %0" : : "r" (SEM_OPEN));

    __asm__ volatile("ecall");

    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int sem_close(sem_t handle){
    __asm__ volatile("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile("mv a0, %0" : : "r" (SEM_CLOSE));

    __asm__ volatile("ecall");

    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int sem_wait(sem_t id){
    __asm__ volatile("mv a1, %[id]" : : [id] "r" (id));
    __asm__ volatile("mv a0, %0" : : "r" (SEM_WAIT));

    __asm__ volatile("ecall");

    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int sem_signal(sem_t id){
    __asm__ volatile("mv a1, %[id]" : : [id] "r" (id));
    __asm__ volatile("mv a0, %0" : : "r" (SEM_SIGNAL));

    __asm__ volatile("ecall");

    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int time_sleep(time_t time) {
    __asm__ volatile("mv a1, %0" : : "r"(time));
    __asm__ volatile("mv a0, %0" : : "r"(TIME_SLEEP));

    __asm__ volatile("ecall");

    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

char getc() {
    __asm__ volatile("mv a0, %0" : : "r"(GETC));
    __asm__ volatile("ecall");

    char volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

void putc(char c) {
    __asm__ volatile("mv a1, %0" : : "r"(c));
    __asm__ volatile("mv a0, %0" : : "r"(PUTC));
    __asm__ volatile("ecall");
}

int thread_add_child(thread_t child){
    __asm__ volatile("mv a1, %0" : : "r"(child));
    __asm__ volatile("mv a0, %0" : : "r"(THREAD_ADD_CHILD));
    __asm__ volatile("ecall");
    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}
int thread_join_all() {
    __asm__ volatile("mv a0, %0" : : "r"(THREAD_JOIN_ALL));
    __asm__ volatile("ecall");
    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}
int getThreadId(){
    __asm__ volatile("mv a0, %0" : : "r"(THREAD_GET_ID));
    __asm__ volatile("ecall");
    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}