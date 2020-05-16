#include "ByteOperations.h"

char* ByteOperations::ushort_to_bytes(uint16_t us) {
    char* a = new char[2];
    a[0] = us & 0xFF;
    a[1] = (us >> 8) & 0xFF;
    return a;
}

uint16_t ByteOperations::bytes_to_ushort(uint8_t MSB, uint8_t LSB){
    return (((uint16_t) MSB) << 8) | LSB;
}

char* ByteOperations::ulong_to_bytes(uint64_t ul) {
    char* a = new char[8];
    for (std::size_t i = 0; i < 8; i++) {
        a[i] = (ul >> (i * 8)) & 0xFF;
    }
    return a;
}

uint32_t ByteOperations::bytes_to_uint(uint8_t ll, uint8_t lr, uint8_t rl, uint8_t rr) {
    return (((uint32_t) ll) << 24) | (((uint32_t) lr) << 16) | (((uint32_t) rl) << 8) | rr;
}

char *ByteOperations::uint_to_bytes(uint32_t ui) {
    char* a = new char[4];
    for (std::size_t i = 0; i < 4; i++) {
        a[i] = (ui >> (i * 8)) & 0xFF;
    }
    return a;
}
