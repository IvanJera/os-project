#ifndef _BUFFER_HPP
#define _BUFFER_HPP

class sem;

class _Buffer {
public:
    _Buffer(int _cap);
    ~_Buffer();

    void put(char val);
    char get();

    int getCnt();
    int getCapacity();

private:
    int capacity;
    char *buffer;
    int head, tail;

    sem* spaceAvailable;
    sem* itemAvailable;
};


#endif
