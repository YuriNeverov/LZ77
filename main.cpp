#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cerrno>
#include <cstring>
#include <climits>
#include <csignal>
#include "LZ77Decoder.h"
#include "lz77_encoder.h"
#include "files.h"

uint compression_power = 50;
bool crashTest = false;

enum class Mode {
    ENCODING,
    DECODING
};

Mode mode;

void printUsage() {
    std::cerr << "Usage:\n";
    std::cerr << "lz77.exe --encode [input file] [output file] [compression power = 50]\n";
    std::cerr << "Or:\n";
    std::cerr << "lz77.exe --decode [input file] [output file]\n";
    std::cerr << "Or:\n";
    std::cerr << "lz77.exe --test [directory] [compression power = 50]\n";
}

bool applyCompressionPower(char * number) {
    long compression = strtol(number, nullptr, 10);
    if ((errno == ERANGE && (compression == LONG_MAX || compression == LONG_MIN)) || (errno != 0 && compression == 0) || compression > 100 || compression < 1) {
        std::cerr << "Wrong compression power argument: should be a number between 1 and 100.\n";
        printUsage();
        return false;
    }
    compression_power = compression;
    return true;
}

bool checkArguments(int argsCount, char *argv[]) {
    if (argsCount > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "help") == 0)) {
        printUsage();
        return true;
    }

    if (argsCount > 1 && strcmp(argv[1], "--test") == 0) {
        if (argsCount == 3) return crashTest = true;
        if (argsCount == 4) return applyCompressionPower(argv[3]);
    }

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
        return applyCompressionPower(argv[4]);
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
    mode = Mode::ENCODING;
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
    mode = Mode::DECODING;
    int decoderExitCode = LZ77Decoder::decode(input_file, output_file);

    return hasDecodingErrors(decoderExitCode);
}

void sigSEGVHandler(int _) {
    if (mode == Mode::DECODING) {
        std::cerr << "Segmentation fault while decoding. Wrong file to decode.\n";
    }
    signal(SIGSEGV, SIG_DFL);
    exit(3);
}

char* addSuffix(std::string str, std::string suffix) {
    char * suffixed = new char[str.size() + suffix.size() + 1];
    std::copy(str.begin(), str.end(), suffixed);
    std::copy(suffix.begin(), suffix.end(), suffixed + str.size());
    suffixed[str.size() + suffix.size()] = '\0';
    return suffixed;
}

void testDirectory(const char * name) {
    std::vector<std::string> filenames;
    readDirectory(name, filenames);
    for (std::string file : filenames) {
        int i = 1;
        bool errFlag = false;
        do {
            i++;

            if (errFlag) break;
            errFlag = true;

            if (crashTest) compression_power = i;

            if (file == "." || file == "..") continue;

            std::string filenm = std::string(name) + '\\' + file;
            std::cout << "Testing " << filenm << "..." << std::endl;

            char *filename = addSuffix(filenm, "");
            char *filename_encoded = addSuffix(filenm, "_encoded");
            char *filename_decoded = addSuffix(filenm, "_decoded");

            performEncoding(filename, filename_encoded);

            if (performDecoding(filename_encoded, filename_decoded)) {
                std::cerr << "Test failed: decoding fell." << std::endl;
                return;
            }

            std::cout << "Decoding finished." << std::endl;

            if (!compareFiles(filename_decoded, filename)) {
                std::cerr << "Test failed: files are not equal: \""
                          << std::string(filename_decoded)
                          << "\" and \""
                          << std::string(filename)
                          << "\"." << std::endl;
                return;
            }
            std::cout << "Test " << filenm << " passed! Compression: "
                      << std::fixed << std::setprecision(2)
                      << ((double)fileSize(filename_encoded))/fileSize(filename) * 100
                      << "%" << std::endl << std::endl << std::endl;

            delete[] filename;

            remove(filename_encoded);
            delete[] filename_encoded;

            remove(filename_decoded);
            delete[] filename_decoded;
            errFlag = false;
        } while (i <= crashTest * 100);
    }
    std::cout << "Tests passed!\n";
}

int main(int argc, char *argv[]) {
    signal(SIGSEGV, sigSEGVHandler);
    if (!checkArguments(argc, argv)) {
        return 1;
    }

    if (strcmp(argv[1], "--test") == 0) testDirectory(argv[2]);

    if (strcmp(argv[1], "--encode") == 0) performEncoding(argv[2], argv[3]);

    if (strcmp(argv[1], "--decode") == 0) performDecoding(argv[2], argv[3]);

    return 0;
}