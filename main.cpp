#include <iostream>
#include <iomanip>
#include "LZ77Decoder.h"
#include "lz77_encoder.h"

constexpr uint compression_power = 20;

// First arg for input file, second for archive, third one for output

int main(int argc, char *argv[]) {
    lz77_encoder lz_encoder;

    clock_t start = clock();

    lz_encoder.encode(argv[1], argv[2], compression_power);

    clock_t end = clock();

    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    std::cout << "Compression time taken by program is : " << std::fixed
         << time_taken << std::setprecision(5);
    std::cout << " sec " << std::endl;


    return LZ77Decoder::decode(argv[2], argv[3]);
}