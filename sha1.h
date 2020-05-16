#ifndef LZ77_SHA1_H
#define LZ77_SHA1_H

#include "ByteOperations.h"
#include <cstdint>
#include <fstream>
#include <string>

class sha1 {
private:
    static constexpr uint32_t k[] = {
            0x5A827999,
            0x6ED9EBA1,
            0x8F1BBCDC,
            0xCA62C1D6
    };

    static constexpr uint32_t init_h[] = {
            0x67452301,
            0xEFCDAB89,
            0x98BADCFE,
            0x10325476,
            0xC3D2E1F0
    };

    static uint32_t f(uint32_t t, uint32_t B, uint32_t C, uint32_t D);

    static uint32_t K(uint32_t t);

    static uint32_t cycleShift(uint32_t X, uint32_t n);

    static void add_one(char* M);

public:
    static void hashFile(const std::string &input_path, uint32_t *h);
};


#endif //LZ77_SHA1_H
