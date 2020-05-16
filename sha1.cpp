#include "sha1.h"

uint32_t sha1::f(uint32_t t, uint32_t B, uint32_t C, uint32_t D) {
    if (t < 20) {
        return (B & C) | ((~B) & D);
    } else if (20 <= t && t < 40) {
        return (B ^ C) ^ D;
    } else if (40 <= t && t < 60) {
        return (B & C) | (B & D) | (C & D);
    } else {
        return (B ^ C) ^ D;
    }
}

uint32_t sha1::K(uint32_t t) {
    return k[t / 20];
}

uint32_t sha1::cycleShift(uint32_t X, uint32_t n) {
    return (X << n) | (X >> (32 - n));
}

// Pre: M[0 .. 63] of bytes
void sha1::add_one(char* M) {
    int rightmost = 63;
    for (; rightmost >= 0; rightmost--) { // Find first from the end non-zero byte
        if (M[rightmost] > 0) break;
    }

    if (rightmost < 0) { // If there is no ones in M
        M[0] |= 0x80;
    }

    int r = M[rightmost];
    int a = 1;
    while (!(a & r)) {
        a <<= 1;
    }
    // a is rightmost one in M[rightmost]
    if (a == 1) {
        if (rightmost == 63) return; // If rightmost one in M is in the end, do nothing
        M[rightmost + 1] |= 0x80;
    } else {
        M[rightmost] |= (a >> 1);
    }
}

// Pre: h[0 .. 4] of uint32_t
void sha1::hashFile(const std::string& input_path, uint32_t *h) {
    for (size_t i = 0; i < 5; i++) {
        h[i] = init_h[i];
    }

    std::ifstream in(input_path, std::ios::binary);

    if (!in.is_open()) {
        throw std::ios::failure("Can`t open input file for hashing!");
    }

    uint64_t length = 0;

    char* M;
    try {
        M = new char[64];
    } catch (std::bad_alloc& e) {
        return;
    }

    bool extra_block = false;
    int read = 0;

    while (!in.eof() || extra_block) {
        std::fill(M, M + 64, 0);
        if (!extra_block) {
            in.read(M, 64);

            read = in.gcount();
            if (read == 0) {
                extra_block = true;
            } else {
                add_one(M);
            }

            length += read;
        }

        if (extra_block || read <= 56) {
            char* len_bytes = ByteOperations::ulong_to_bytes(length);
            for (size_t i = 0; i < 8; i++) {
                M[56 + i] = len_bytes[7 - i];
            }
            delete[] len_bytes;
        }

        uint32_t w[80];
        for (size_t i = 0; i < 16; i++) {
            w[i] = ByteOperations::bytes_to_uint(M[i * 4], M[i * 4 + 1], M[i * 4 + 2], M[i * 4 + 3]);
        }
        for (size_t i = 16; i < 80; i++) {
            w[i] = cycleShift(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }

        uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
        for (size_t i = 0; i < 80; i++) {
            uint32_t temp = cycleShift(a, 5) + f(i, b, c, d) + e + w[i] + K(i);
            e = d;
            d = c;
            c = cycleShift(b, 30);
            b = a;
            a = temp;
        }

        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
        h[4] += e;

        if (extra_block) break;

        if (56 < read && read < 64) extra_block = true;
    }

    delete M;
}
