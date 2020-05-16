#pragma GCC optimize("O3")

#include <iostream>

typedef unsigned char uchar;
typedef unsigned int uint;

class array_deque {
public:
    array_deque() {
        length = 2, num = 0;
        elements = new uchar[2];
        tail = 0, head = 0;
    }

    //insert in begin
    void push(uchar element);

    //get first element
    uchar peek();

    //get and delete first element
    uchar remove();

    //insert in the end
    void enqueue(uchar element);

    //get last element
    uchar element();

    //get and delete last element
    uchar dequeue();

    //get size
    int size();

    //check for empty
    bool empty();

    // get x element
    uchar& operator[](uint);

    //check for deque`s array size
    void resize_up();

    //clear deque
    void clear() {
        delete[] elements;
    }

private:
    int up(int x) {
        if (x < 0) {
            x += length;
        }
        return x;
    }

    int down(int x) {
        if (x >= length) {
            x -= length;
        }
        return x;
    }

    int length, num;
    uchar *elements;
    int tail, head;
};