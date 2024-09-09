#pragma once 
#ifndef IsaLLVM_File
#define IsaLLVM_File
#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>


static std::string removeSpaces(const std::string& input) {
    auto it = std::find_if(input.begin(), input.end(), [](char ch) {
        return !std::isspace(ch); 
    });
    return std::string(it, input.end());
}

static std::vector<std::string> splitByErr(const std::string& str) {
    std::vector<std::string> columns;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, '\n')) {
        columns.push_back(removeSpaces(item));
    }

    return columns;
}

static std::string fileopen(const std::string& filename) {
    std::fstream fs(filename, std::fstream::in);
    std::stringstream ss;
    if(!fs.is_open()) {
        throw std::runtime_error("Não foi possivel abrir o arquivo! " + filename);
    }
    ss << fs.rdbuf();
    fs.close();
    return ss.str();
}

#endif // !




