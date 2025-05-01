#pragma once 
#include "flags.hpp"
#include "frontend/utils/initialized_flags.hpp"
#include "frontend/utils/results.hpp"
#ifndef IsaLLVM_File
#define IsaLLVM_File
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

static std::string splitByStr(const std::string& str) {
    std::vector<std::string> columns;
    std::stringstream ss(str);
    std::string item;
    std::string env;

    while (std::getline(ss, item, '\n')) {
        env += removeSpaces(item) + '\n';
    }

    return env;
}

inline Result<std::string> fileopen(const std::string& filename) {
    std::ifstream fs(filename);
    if (!fs.is_open()) {
        return Result<std::string>::Err("Could not open file: " + filename + '\n');
    }
    std::stringstream ss;
    ss << fs.rdbuf();
    return Result<std::string>::Ok(ss.str());
}

inline Result<std::string> get_input_filename() {
    if (flags.isActive(FlagID::RunInputFile) || flags.isActive(FlagID::BuildInputFile)) {
        std::string filename = flags.getValue({FlagID::RunInputFile,FlagID::BuildInputFile}).unwrap();
        if (!filename.empty()) {
            return filename;
        } else {
            return Result<std::string>::Err("Invalid filename after -o");
        }
    }


    return Result<std::string>::Err("No input file specified.\n");
}

inline std::string get_output_filename() {
    if (flags.isActive(FlagID::OutputFile)) {
        std::string outputname = flags.getValue(FlagID::OutputFile).unwrap();
        if (!outputname.empty()) {
            return outputname;
        }
    }
    return "out.ll";
}

#endif // !
