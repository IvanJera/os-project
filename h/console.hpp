#ifndef _CONSOLE_HPP
#define _CONSOLE_HPP

#include "../lib/hw.h"

class console {
public:
    static void init();
    static void putc(char c);
    static char getc();

    static void rxFill();
    static void txDrain();

    static bool inputEmpty()  { return inCount == 0; }
    static bool outputEmpty() { return outCount == 0; }

private:
    static const int CAP = 256;

    static char inBuf[CAP];
    static int  inHead, inTail, inCount;

    static char outBuf[CAP];
    static int  outHead, outTail, outCount;
};

#endif