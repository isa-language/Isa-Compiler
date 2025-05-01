#pragma once 

#include <cstdio>
#ifdef _WIN32
    #include <windows.h>
#endif
static void print_progress(long long current, long long total) {
    int percent = (int)((current * 100) / total);
    int bars = percent / 2;

    char linha[100];
    char *p = linha;
    p += sprintf(p, "\rProgresso: [");

    for (int i = 0; i < 50; i++) {
        *p++ = (i < bars) ? '#' : ' ';
    }
    p += sprintf(p, "] %d%%", percent);
    *p = '\0';

    fputs(linha, stdout);
    fflush(stdout);
}