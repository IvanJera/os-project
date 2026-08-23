#include "../h/syscall_cpp.hpp"

void* operator new (size_t n) {
    return mem_alloc(n);
}

void* operator new[] (size_t n) {
    return mem_alloc(n);
}

void operator delete (void* ptr) {
    if(!ptr) return;
    mem_free(ptr);
}

void operator delete[] (void* ptr) {
    if(!ptr) return;
    mem_free(ptr);
}

Thread::Thread (void (*body)(void*), void* arg) {
    this->body = body;
    this->arg = arg;
}

Thread::Thread () {
    this->body = &(Thread::wrapper);
    this->arg = this;
}

Thread::~Thread () {
    uint64* ptr = (uint64*) myHandle;
    delete[] ptr;
}

int Thread::start () {
    if(arg == this) return thread_create(&myHandle, wrapper, arg);
    else return thread_create(&myHandle, body, arg);
}

void Thread::dispatch () {
    thread_dispatch();
}

int Thread::sleep (time_t t) {
    return time_sleep(t);
}

Semaphore::Semaphore (unsigned init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore () {
    sem_close(myHandle);
}

int Semaphore::wait () {
    return sem_wait(myHandle);
}

int Semaphore::signal () {
    return sem_signal(myHandle);
}

void PeriodicThread::terminate() {
    this->period = -1;
}

PeriodicThread::PeriodicThread(time_t period) {
    this->period = period;
}

void PeriodicThread::run() {
    while(this->period > 0) {
        this->sleep(this->period);
        this->periodicActivation();
    }
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}

int Thread::addChild(Thread *child) {
    return thread_add_child(child->myHandle);
}

int Thread::joinAll() {
    return thread_join_all();
}
int Thread::getId() {
    return getThreadId();
}