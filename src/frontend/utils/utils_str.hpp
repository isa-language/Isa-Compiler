#pragma once 
#ifndef UTILS
#define UTILS

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <sstream>
#include <fcntl.h>

static inline std::wstring char_to_wstring(const char* input) {
    if (!input) return L"";

    int inputLength = static_cast<int>(strlen(input));

    int size = MultiByteToWideChar(CP_UTF8, 0, input, inputLength, nullptr, 0);
    if (size == 0) return L"";
    
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, input, inputLength, &wstr[0], size);
    wstr.pop_back();
    return wstr;
}

static inline std::wstring string_to_wstring(const std::string& str) {
    if (str.empty()) return L"";
    
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    if (size == 0) return L"";
    
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
    wstr.pop_back();
    return wstr;
}

static inline std::wstring int_to_wstring(int value) {
    std::wstringstream wss;
    wss << value;
    return wss.str();
}


#endif // !SYS
#endif // !UTILS
