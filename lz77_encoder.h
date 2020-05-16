#pragma GCC optimize("O3")

#include <fstream>
#include <cmath>
#include <algorithm>
#include "array_deque.h"
#include <exception>
#include "sha1.h"

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;

class lz77_encoder {
public:
    lz77_encoder() {
        block_buffer = new uchar[BUFFER_SIZE];
    }

    void open_input_file(char *input_file);

    void open_output_file(char *output_file);

    void encode(char *input_file, char *output_file, uint speed, std::string type = "TNA2");

    void read_block();

    ~lz77_encoder() {
        delete[] block_buffer;
        delete hash;
        buffer.clear();
        window.clear();
        input.close();
        output.close();
    }

private:
    const ushort BUFFER_SIZE = 4 * 1024;
    uint WINDOW_SIZE;

    std::ifstream input;
    std::ofstream output;
    array_deque window, buffer;
    uchar *block_buffer;
    ushort buffer_size_processing;
    uint file_size;
    uint *hash;

    std::pair<ushort, ushort> findMatching();

    uchar get(uint k);

    void get_file_size();

    void shift_buffer(uint shift);

    void ushort_to_bytes(ushort, char*);

    void set_window_size(uint size, bool is_tna1);
};
