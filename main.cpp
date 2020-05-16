#include <iostream>
#include <iomanip>
#include "LZ77Decoder.h"
#include "lz77_encoder.h"

constexpr uint compression_power = 20;

bool checkDecodingErrors(int decoderExitCode) {
    switch (decoderExitCode) {
        case 1:
            std::cerr << "Cannot open input file for decoding\n";
            return true;
        case 2:
            std::cerr << "Cannot open output file for decoding\n";
            return true;
        case 4:
            std::cerr << "Bad memory allocation while decoding\n";
            return true;
        case 8:
            std::cerr << "Wrong file to decode: hashes don't match\n";
            return true;
        default:
            return false;
    }
}

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

    int decoderExitCode = LZ77Decoder::decode(argv[2], argv[3]);

    if (checkDecodingErrors(decoderExitCode))
        return 1;

    return 0;
}