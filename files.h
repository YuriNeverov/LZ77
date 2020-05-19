//
// Created by Christopher on 18.05.2020.
//

#ifndef LZ77_FILES_H
#define LZ77_FILES_H

#include "dirent.h"
#include <fstream>
#include <vector>
#include <string>

void readDirectory(const std::string& name, std::vector<std::string>& v);

bool compareFiles(const std::string& left, const std::string& right);

int64_t fileSize(const std::string& name);

#endif //LZ77_FILES_H
