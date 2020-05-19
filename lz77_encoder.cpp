#include "lz77_encoder.h"

void lz77_encoder::open_input_file(char *input_file) {
    hash = new uint[5];
    try {
        sha1::hashFile(input_file, hash);
    } catch (std::ios::failure &e) {
        throw std::ios::failure("Can`t open input file for hashing!");
    }

    input.open(input_file, std::ios::binary);

    if (!input.is_open()) {
        throw std::ios_base::failure("Can`t open input file!");
    }
}

void lz77_encoder::open_output_file(char *output_file) {
    output.open(output_file, std::ios::binary);

    if (!output.is_open()) {
        throw std::ios::failure("Can`t open output file!");
    }
}

void lz77_encoder::read_block() {
    uint size = input.readsome(reinterpret_cast<char *>(block_buffer), BUFFER_SIZE);
    for (uint i = 0; i < size; i++) {
        buffer.enqueue(block_buffer[i]);
    }
}

void lz77_encoder::encode(char *input_file, char *output_file, uint speed, std::string type) {
    if (type != "TNA1" && type != "TNA2") {
        std::cerr << "Type should be TNA1 or TNA2!";
        return;
    }
    
    open_input_file(input_file);
    open_output_file(output_file);

    uint pos = 0;
    get_file_size();
    read_block();
    bool is_tna1 = (type == "TNA1");
    char *bytes = new char[2];
    set_window_size(speed, is_tna1);

    is_tna1 ? output << "TNA1" : output << "TNA2";

    output << " " << WINDOW_SIZE << " ";
    for (uint i = 0; i < 5; i++) {
        output.write(ByteOperations::uint_to_bytes(hash[i]), 4);
    }
    output << " ";

    while (pos < file_size) {
        std::pair<ushort, ushort> res = findMatching();
        uint safe = pos;

        if (pos + res.second > file_size - 1) {
            res.second = file_size - pos - 1;
        }

//        pos + res.second > file_size ? shift_buffer(res.second) : shift_buffer(res.second + 1);
//        pos += res.second + 1;
        shift_buffer(res.second + 1);
        if (res.second + pos >= file_size) {
            std::cout << safe << " " << pos << " " << file_size << " " << res.first << " " << res.second << std::endl;
//            res.second -= (ushort)(pos - file_size);
//            pos = file_size + 1;
        }

        if (is_tna1) {
            output << (uchar) res.first << (uchar) res.second;
        } else {
            ushort_to_bytes(res.first, bytes);
            output.write(bytes, 2);

            ushort_to_bytes(res.second, bytes);
            output.write(bytes, 2);
        }

//        if (pos < file_size) {
            output << window.peek();
//        }

//        if (pos > file_size) {
//            break;
//        }

        pos += res.second + 1;
    }

    delete[] bytes;
    delete[] hash;
    output.close();
    input.close();
}

void lz77_encoder::ushort_to_bytes(ushort in, char *out) {
    out[0] = in & 0xFF;
    out[1] = (in >> 8) & 0xFF;
}

std::pair<ushort, ushort> lz77_encoder::findMatching() {
    buffer_size_processing = std::min(buffer.size(), window.size());
    const uint size = buffer_size_processing + window.size() + 1;

    ushort *pi;

    try {
        pi = new ushort[size];
        pi[0] = 0;
        pi[buffer_size_processing] = 0;
    } catch (std::bad_alloc &e) {
        std::cerr << "Can`t create buffer for window processing!\n" << e.what();
        throw std::bad_alloc();
    }

    ushort tmp_val;

    for (ushort i = 1; i < buffer_size_processing; i++) {
        tmp_val = pi[i - 1];
        while (tmp_val > 0 && buffer[i] != buffer[tmp_val]) {
            tmp_val = pi[tmp_val - 1];
        }
        if (buffer[i] == buffer[tmp_val]) {
            tmp_val++;
        }
        pi[i] = tmp_val;
    }

    ushort offset = buffer_size_processing + 1, length = 0;

    for (ushort i = buffer_size_processing + 1; i < size; i++) {
        tmp_val = pi[i - 1];
        while (tmp_val > 0 && tmp_val != buffer_size_processing && window[i - offset] != get(tmp_val)) {
            tmp_val = pi[tmp_val - 1];
        }
        if (tmp_val != buffer_size_processing &&  window[i - offset] == get(tmp_val)) {
            tmp_val++;
        }
        pi[i] = tmp_val;
    }

    for (ushort i = buffer_size_processing + 1; i < size; i++) {
        if (pi[i] > length) {
            offset = i;
            length = pi[i];
        }
    }
    delete pi;
    return std::pair<ushort, ushort>(length == 0 ? 0 : size - offset + length - 1, length);
}

void lz77_encoder::shift_buffer(uint shift) {
    if ((uint)buffer.size() < shift) {
        read_block();
    }

    ushort dif = shift + window.size() > WINDOW_SIZE ? shift + window.size() - WINDOW_SIZE : 0;

    for (ushort i = 0; i < std::min(window.size(), (int) dif); i++) {
        window.dequeue();
    }

    ushort full = shift / WINDOW_SIZE, ost = shift % WINDOW_SIZE;

    if (full > 0) {
        for (ushort i = 0; i < WINDOW_SIZE; i++) {
            window.enqueue(buffer[ost + i]);
        }
    } else {
        for (ushort i = 0; i < shift; i++) {
            window.enqueue(buffer[i]);
        }
    }

    for (ushort i = 0; i < shift; i++) {
        buffer.dequeue();
    }

    if (buffer.size() < window.size()) {
        read_block();
    }
}

uchar lz77_encoder::get(uint k) {
    if (k > buffer_size_processing) {
        return window[k - buffer_size_processing - 1];
    } else {
        return buffer[k];
    }
}

void lz77_encoder::get_file_size() {
    input.seekg(0, std::ifstream::end);
    file_size = input.tellg();
    input.seekg(0, std::ifstream::beg);
}

void lz77_encoder::set_window_size(uint speed, bool is_tna1) {
    speed = std::min((uint)100, speed);

    if (is_tna1) {
        WINDOW_SIZE = 155 + (100 * speed) / 100;
    } else {
        WINDOW_SIZE = 1024 + ((3 * 1024) * speed) / 100;
    }
}
