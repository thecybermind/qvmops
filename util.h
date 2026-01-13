/*
QVMOPS - Quake3 Virtual Machine Opcodes disassembler
Copyright 2004-2026
https://github.com/thecybermind/qvmops/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < k.m.masterson@gmail.com >

*/

#pragma once
#ifndef __QVMOPS_UTIL_H__
#define __QVMOPS_UTIL_H__

#include <stdint.h>
#include <stdio.h>

// "safe" strncpy that always null-terminates
char* strncpyz(char* dest, const char* src, size_t count);

// "safe" strncat where count is the total size of dest, and always null-terminates WITHIN the buffer
char* strncatz(char* dest, const char* src, size_t count);

// return a printable character for c ('.' for unprintable)
char printablec(uint8_t c);

// reverse version of strstr (search backwards from end of string)
char* strrstr(const char* str, const char* substr);

#ifdef _MSC_VER
#define MINIMUM_BUFFER_SIZE 128
typedef intptr_t ssize_t;
ssize_t getline(char** lineptr, size_t* n, FILE* stream);
#endif

#endif // __QVMOPS_UTIL_H__
