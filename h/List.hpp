#ifndef _LIST_HPP
#define _LIST_HPP

#include "../h/MemoryAllocator.hpp"
#include "../test/printing.hpp"

template<typename T>
class List {
public:
    List() : first(nullptr), last(nullptr) {}
    List(const List<T>&) = delete;
    List<T>& operator=(const List<T>&) = delete;

    void addFirst(T* element) {
        Link* link = new Link(element, first);
        first = link;
        if (last == nullptr) last = link;
    }

    void addLast(T* element) {
        Link* link = new Link(element, nullptr);
        if (last == nullptr) {
            first = last = link;
        } else {
            last->succ = link;
            last = link;
        }
    }

    T* removeFirst() {
        if (first == nullptr) return nullptr;
        Link* link = first;
        first = first->succ;
        if (first == nullptr) last = nullptr;
        T* payload = link->payload;
        delete link;
        return payload;
    }

    T* removeLast() {
        if (last == nullptr) return nullptr;

        // pronadji pretposlednji cvor
        Link* beforeLast = nullptr;
        for (Link* cursor = first; cursor != last; cursor = cursor->succ) {
            beforeLast = cursor;
        }

        Link* link = last;
        last = beforeLast;
        if (beforeLast != nullptr) beforeLast->succ = nullptr;
        else first = nullptr;

        T* payload = link->payload;
        delete link;
        return payload;
    }

    T* peekFirst() {
        return first ? first->payload : nullptr;
    }

    T* peekLast() {
        return last ? last->payload : nullptr;
    }
	T* peekAt(int i){
        Link* temp=first;
        while(temp&&i>0){temp=temp->succ;i--;}
        return temp?temp->payload:nullptr;
    }

    void remove(T* element) {
        Link* prevLink = nullptr;
        Link* cursor = first;
        while (cursor != nullptr) {
            if (cursor->payload == element) {
                if (prevLink != nullptr) prevLink->succ = cursor->succ;
                else first = cursor->succ;
                if (cursor == last) last = prevLink;
                delete cursor;
                return;
            }
            prevLink = cursor;
            cursor = cursor->succ;
        }
    }

    static void* operator new(size_t size) {
        return MemoryAllocator::mem_alloc(size);
    }
    static void* operator new[](size_t size) {
        return MemoryAllocator::mem_alloc(size);
    }
    static void operator delete(void* ptr) {
        MemoryAllocator::mem_free(ptr);
    }
    static void operator delete[](void* ptr) {
        MemoryAllocator::mem_free(ptr);
    }

private:
    struct Link {
        T* payload;
        Link* succ;

        Link(T* payload, Link* succ) : payload(payload), succ(succ) {}

        static void* operator new(size_t size) {
            return MemoryAllocator::mem_alloc(size);
        }
        static void* operator new[](size_t size) {
            return MemoryAllocator::mem_alloc(size);
        }
        static void operator delete(void* ptr) {
            MemoryAllocator::mem_free(ptr);
        }
        static void operator delete[](void* ptr) {
            MemoryAllocator::mem_free(ptr);
        }
    };

    Link* first;
    Link* last;
};

#endif //_LIST_HPP