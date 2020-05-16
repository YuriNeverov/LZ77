#include "LZ77Decoder.h"
#include <iostream>
void LZ77Decoder::readTriad(char* byte_buffer, uint16_t triad_size, bool empty, uint16_t& offset, uint16_t& count, char& ch) {
    switch (triad_size) {
        case 3:
            offset = byte_buffer[0];
            count = byte_buffer[1];
            break;
        case 5:
            offset = ByteOperations::bytes_to_ushort(byte_buffer[1], byte_buffer[0]);
            count = ByteOperations::bytes_to_ushort(byte_buffer[3], byte_buffer[2]);
            break;
    }

    if (!empty) {
        ch = byte_buffer[triad_size - 1];
    }
}

// Pre: max_buffer_size < 4 * output_buffer_size
int LZ77Decoder::decode(const std::string &input_path, const std::string &output_path) {
    size_t triad_size;

    std::ifstream in(input_path, std::ios::binary);
    if (!in) {
        return 1; // Input file not opened
    }

    std::ofstream out(output_path, std::ios::binary);
    if (!out) {
        return 2; // Output file not opened
    }

    // TNA[1|2] [min_window_buffer_size; min_window_buffer_size - 1 + 2^8|2^16] [hash]
    std::string format;
    size_t window_buffer_size;
    uint32_t hash[5]; // SHA1 hash 5 * 32 = 160 bits

    in >> format >> window_buffer_size;
    in.ignore();
    window_buffer_size += min_window_buffer_size;

    // TNA1 or TNA2
    if (format[3] == '1') {
        triad_size = 3;
    } else {
        triad_size = 5;
    }

    char hash_buffer[20]; // 20 * 8 bits
    in.read(hash_buffer, 20);
    for (size_t i = 0; i < 5; i++) {
        hash[i] = ByteOperations::bytes_to_uint(hash_buffer[4 * i + 3], hash_buffer[4 * i + 2],
                hash_buffer[4 * i + 1], hash_buffer[4 * i]);
    }

    // input_buffer_size = [number of bytes in a triad] * [number of triads per block]
    size_t input_buffer_size = triad_size * input_buffer_triads;
    char *output_buffer, *input_buffer;
    try {
        output_buffer = new char[output_buffer_size + 2 * window_buffer_size];
        input_buffer = new char[input_buffer_size];
    } catch (std::bad_alloc& e) {
        return 4; // Bad alloc
    }
    size_t output_buffer_iterator = 0;
    // Ignore whitespace after hash
    in.ignore();

    while (!in.eof()) {
        // LZ77 (offset, count, char)
        uint16_t offset, count;
        char ch;
        in.read(input_buffer, input_buffer_size);

        // Check if we actually have read something. If not, break
        size_t read = in.gcount();
        if (read == 0) break;

        // Number of triad read, including empty char
        auto triad_read = static_cast<size_t>(ceil((double)read / triad_size));
        // Number of triads read, excluding empty char
        size_t full_triad_read = read / triad_size;
        // Block has empty char in the end iff those numbers don't match
        bool empty_block = full_triad_read != triad_read;


        for (size_t input_buffer_iterator = 0; input_buffer_iterator < triad_read; input_buffer_iterator++) {
            // Current triad has empty char iff block is empty and this is the last triad in the block
            bool empty_char = empty_block && input_buffer_iterator == triad_read - 1;
            // Pointer to current triad
            char* cur_triad = input_buffer + input_buffer_iterator * triad_size;

            readTriad(cur_triad, triad_size, empty_char, offset, count, ch);

            // LZ77 triad decoding
            size_t start = output_buffer_iterator - offset;
            for (size_t i = 0; i < count; i++) {
                output_buffer[output_buffer_iterator + i] = output_buffer[start + i];
            }
            output_buffer_iterator += count;

            // Add char in the end iff it's not empty
            if (!empty_char) {
                output_buffer[output_buffer_iterator++] = ch;
            }

            // Flush if output_buffer_iterator stepped over output buffer zone and window buffer zone
            if (output_buffer_iterator >= output_buffer_size + window_buffer_size) {
                out.write(output_buffer, output_buffer_size);
                size_t j = 0;
                for (size_t i = output_buffer_size; i < output_buffer_iterator; i++, j++) {
                    output_buffer[j] = output_buffer[i];
                }
                output_buffer_iterator = j;
            }
        }
    }

    // Flush what we haven't flush yet
    out.write(output_buffer, output_buffer_iterator);

    // Free memory space
    delete[] input_buffer;
    delete[] output_buffer;

    // Closing IO streams
    in.close();
    out.close();

    uint32_t new_hash[5];
    sha1::hashFile(output_path, new_hash);

    for (size_t i = 0; i < 5; i++) {
        if (new_hash[i] != hash[i]) return 8;
    }

    // No exceptions so far
    return 0;
}


