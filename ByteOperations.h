#ifndef LZ77_BYTEOPERATIONS_H
#define LZ77_BYTEOPERATIONS_H

#include <cstdint>

class ByteOperations {
public:
    static uint16_t bytes_to_ushort(uint8_t MSB, uint8_t LSB);

    static uint32_t bytes_to_uint(uint8_t ll, uint8_t lr, uint8_t rl, uint8_t rr);

    static char* ushort_to_bytes(uint16_t us);

    static char* ulong_to_bytes(uint64_t ul);

    static char* uint_to_bytes(uint32_t ui);
};


#endif //LZ77_BYTEOPERATIONS_H
