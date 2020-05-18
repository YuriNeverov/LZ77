//
// Created by Christopher on 18.05.2020.
//
#include "files.h"

void readDirectory(const std::string& name, std::vector<std::string>& v)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != nullptr) {
        v.emplace_back(dp->d_name);
    }
    closedir(dirp);
}

bool compareFiles(const std::string& left, const std::string& right) {
    int TEST_BUFFER_SIZE = 1024;

    std::ifstream lin(left, std::ios::binary);
    if (!lin) {
        return false;
    }

    std::ifstream rin(right, std::ios::binary);
    if (!rin) {
        return false;
    }

    char lbuffer[TEST_BUFFER_SIZE], rbuffer[TEST_BUFFER_SIZE];

    while (!lin.eof()) {
        if (rin.eof()) return false;

        lin.read(lbuffer, TEST_BUFFER_SIZE);
        rin.read(rbuffer, TEST_BUFFER_SIZE);

        int lcnt = lin.gcount(), rcnt = rin.gcount();

        if (lcnt != rcnt) {
            return false;
        }

        if (lcnt == 0) break;

        for (int i = 0; i < lcnt; i++) if (lbuffer[i] != rbuffer[i]) return false;
    }

    lin.close();
    rin.close();

    return true;
}

