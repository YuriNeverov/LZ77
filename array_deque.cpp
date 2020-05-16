#include "array_deque.h"

void array_deque::push(uchar element) {
    resize_up();
    tail = up(tail - 1);
    elements[tail] = element;
    num++;
}

uchar array_deque::peek() {
    return elements[up(head - 1)];
}

uchar array_deque::remove() {
    uchar answer = peek();
    head = up(head - 1);
    num--;
    return answer;
}

void array_deque::enqueue(uchar element) {
    resize_up();
    elements[head] = element;
    num++;
    head = down(head + 1);
}

uchar array_deque::element() {
    return elements[tail];
}

uchar array_deque::dequeue() {
    uchar answer = element();
    tail = down(tail + 1);
    num--;
    return answer;
}

int array_deque::size() {
    return num;
}

bool array_deque::empty() {
    return num == 0;
}

uchar &array_deque::operator[](uint index) {
    return elements[down(tail + index)];
}

void array_deque::resize_up() {
    if (num < length) return;

    uchar *newDeque;
    try {
        newDeque = new uchar[length * 2];
    } catch(std::bad_alloc &e) {
        std::cerr << "Can`t create new Deque\n" << e.what();
        throw std::bad_alloc();
    }

    int it = 0;
    for (int i = tail; i != head || it == 0; i = down(i + 1), it++) {
        newDeque[it] = elements[i];
    }

    delete []elements;
    elements = newDeque;
    length *= 2;
    head = it;
    tail = 0;
}
