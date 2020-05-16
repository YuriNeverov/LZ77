#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cerrno>
#include <cstring>
#include <climits>
#include "LZ77Decoder.h"
#include "lz77_encoder.h"

uint compression_power = 50;

void printUsage() {
    std::cerr << "Usage:\n";
    std::cerr << "lz77.exe --encode [input file] [output file] [compression power = 50]\n";
    std::cerr << "Or:\n";
    std::cerr << "lz77.exe --decode [input file] [output file]\n";
}

bool checkArguments(int argsCount, char *argv[]) {
    if (argsCount < 4) {
        std::cerr << "Not enough arguments.\n";
        printUsage();
        return false;
    }

    if (strcmp(argv[1], "--encode") != 0 && strcmp(argv[1], "--decode") != 0) {
        std::cerr << "Wrong mode argument.\n";
        printUsage();
        return false;
    }

    if (strcmp(argv[1], "--encode") == 0 && argsCount == 5) {
        long compression = strtol(argv[4], nullptr, 10);
        if ((errno == ERANGE && (compression == LONG_MAX || compression == LONG_MIN)) || (errno != 0 && compression == 0) || compression > 100 || compression < 1) {
            std::cerr << "Wrong compression power argument: should be a number between 1 and 100.\n";
            printUsage();
            return false;
        }
        compression_power = compression;
    }

    return true;
}

bool hasDecodingErrors(int decoderExitCode) {
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

void performEncoding(char* input_file, char* output_file) {
    lz77_encoder lz_encoder;
    std::cout << "Encoding started with compression power " << compression_power << ".\n";

    clock_t start = clock();

    lz_encoder.encode(input_file, output_file, compression_power);

    clock_t end = clock();

    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    std::cout << "Compression time taken by program is : " << std::fixed
              << time_taken << std::setprecision(5);
    std::cout << " sec " << std::endl;
}

bool performDecoding(char* input_file, char* output_file) {
    int decoderExitCode = LZ77Decoder::decode(input_file, output_file);

    return hasDecodingErrors(decoderExitCode);
}

int main(int argc, char *argv[]) {
    if (!checkArguments(argc, argv)) {
        return 1;
    }

    if (strcmp(argv[1], "--encode") == 0) performEncoding(argv[2], argv[3]);

    if (strcmp(argv[1], "--decode") == 0) performDecoding(argv[2], argv[3]);

    return 0;
}