#include "../h/console.hpp"
#include "../h/syscall_c.hpp"

char console::inBuf[CAP];
int  console::inHead = 0, console::inTail = 0, console::inCount = 0;

char console::outBuf[CAP];
int  console::outHead = 0, console::outTail = 0, console::outCount = 0;

void console::init() {
    inHead = inTail = inCount = 0;
    outHead = outTail = outCount = 0;
}

void console::putc(char c) {
    while (outCount >= CAP) {
        txDrain();
    }
    outBuf[outTail] = c;
    outTail = (outTail + 1) % CAP;
    outCount++;
    txDrain();
}

char console::getc() {

    while (inCount == 0) {
        thread_dispatch();
    }
    char c = inBuf[inHead];
    inHead = (inHead + 1) % CAP;
    inCount--;
    return c;
}

void console::rxFill() {
    while ((*((char*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT) && inCount < CAP) {
        char c = *(char*)CONSOLE_RX_DATA;
        inBuf[inTail] = c;
        inTail = (inTail + 1) % CAP;
        inCount++;
    }
}

void console::txDrain() {
    while ((*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT) && outCount > 0) {
        char c = outBuf[outHead];
        outHead = (outHead + 1) % CAP;
        outCount--;
        *(volatile char*)CONSOLE_TX_DATA = c;
    }
}