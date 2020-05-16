#ifndef LZ77_LZ77DECODER_H
#define LZ77_LZ77DECODER_H

#include "ByteOperations.h"
#include <cmath>
#include <cstdint>
#include <fstream>
#include "sha1.h"
#include <string>

class LZ77Decoder {
private:
    const static size_t output_buffer_size = 2048,
            input_buffer_triads = 500,
            min_window_buffer_size = 0;

    static void readTriad(char* byte_buffer, uint16_t triad_size, bool empty, uint16_t& offset, uint16_t& count, char& ch);
public:
    /*
     * Returns error code.
     * 0 is for "success"
     * 1 is for "input-file open failed"
     * 2 is for "output-file open failed"
     * 4 is for "bad alloc"
     * 8 is for "decoded file hash differs from original one's hash"
     */
    static int decode(const std::string &input_path, const std::string &output_path);
};


#endif //LZ77_LZ77DECODER_H
