/*
QVMOPS - Quake3 Virtual Machine Opcodes disassembler
Copyright 2004-2026
https://github.com/thecybermind/qvmops/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
	Kevin Masterson < k.m.masterson@gmail.com >

*/

#pragma once
#ifndef QVMOPS_SYMBOLS_H
#define QVMOPS_SYMBOLS_H

#include "qvm.h"

#define MAX_SYMBOLS	5000
typedef struct symbolmap_s {
	int index;
	int segment;
	int offset;
	char* symbol;
} symbolmap_t;
extern symbolmap_t symbols[SEGMENT_COUNT][MAX_SYMBOLS];
extern int symbolcount[SEGMENT_COUNT];

#define MAX_LINES 20000
extern symbolmap_t lines[MAX_LINES];
extern int linecount;

// find a symbol by offset (after given symbol index)
symbolmap_t* find_line(int index, int after);
symbolmap_t* find_code_symbol(int index, int after);
symbolmap_t* find_data_symbol(int offset, int after);

// fill symbols array with data from map file
void parse_map(const char* file);

#endif // QVMOPS_SYMBOLS_H
