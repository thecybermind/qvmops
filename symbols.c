/*
QVMOPS - Quake3 Virtual Machine Opcodes disassembler
Copyright 2004-2026
https://github.com/thecybermind/qvmops/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < k.m.masterson@gmail.com >

*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "symbols.h"
#include "util.h"

symbolmap_t symbols[SEGMENT_COUNT][MAX_SYMBOLS];
int symbolcount[SEGMENT_COUNT];


static symbolmap_t parse_map_line_ex(char* line);
static symbolmap_t parse_map_line(char* line);


void parse_map(const char* file) {
	FILE* h;
	char* line = NULL;
	size_t linelen = 0;
	ssize_t ret;
	int segment = 0;

	printf("Opening %s...\n", file);

	h = fopen(file, "r");
	if (!h || feof(h) || ferror(h)) {
		fprintf(stderr, "File not found: %s\n", file);
		goto fail;
	}

	while ((ret = getline(&line, &linelen, h)) != -1) {
		// empty line
		if (!line || !*line || !linelen)
			continue;
		symbolmap_t symbol = parse_map_line(line);
		segment = symbol.segment;
		// invalid line
		if (segment < 0)
			continue;

		symbol.index = symbolcount[segment];

		// store in list
		symbols[segment][symbolcount[segment]] = symbol;

		symbolcount[segment]++;
	}

	fclose(h);
	return;

fail:
	if (h)
		fclose(h);
}


// find a symbol by instruction index (after given symbol index)
symbolmap_t* find_code_symbol(int index, int after) {
	int segment = SEGMENT_CODE;

	if (!symbolcount[segment] || after < -1 || after > symbolcount[segment])
		return NULL;

	// find the symbol with the highest offset that is less than the desired one
	int likely_symbol = -1;
	for (int i = after + 1; i < symbolcount[segment]; i++) {
		if (symbols[segment][i].offset == index)
			return &symbols[segment][i];

		if (symbols[segment][i].offset < index) {
			if (likely_symbol < 0 || symbols[segment][i].offset > symbols[segment][likely_symbol].offset)
				likely_symbol = i;
		}
	}

	if (likely_symbol < 0)
		return NULL;

	return &symbols[segment][likely_symbol];
}


// find a symbol by offset (after given symbol index)
// we have to check offset vs segment lengths, etc
symbolmap_t* find_data_symbol(int offset, int after) {
	int segment = SEGMENT_DATA;

	if (offset > datasize[SEGMENT_DATA]) {
		segment = SEGMENT_LIT;
		offset -= datasize[SEGMENT_DATA];
	}

	if (offset > datasize[SEGMENT_LIT]) {
		segment = SEGMENT_BSS;
		offset -= datasize[SEGMENT_LIT];
	}

	// find the symbol with the highest offset that is less than the desired one
	int likely_symbol = -1;
	for (int i = after + 1; i < symbolcount[segment]; i++) {
		if (symbols[segment][i].offset == offset)
			return &symbols[segment][i];

		if (symbols[segment][i].offset < offset) {
			if (likely_symbol < 0 || symbols[segment][i].offset > symbols[segment][likely_symbol].offset)
				likely_symbol = i;
		}
	}

	if (likely_symbol < 0)
		return NULL;

	return &symbols[segment][likely_symbol];
}


// parse a line from the .map file from stvoymp, which adds extra stuff
static symbolmap_t parse_map_line_ex(char* line) {
	char buf[4][256] = { 0, };
	symbolmap_t ret = {
		-1,		// index
		-1,		// segment
		0,		// offset
		NULL,	// symbol
	};
	int n = sscanf(line, " %256s %256s %256s %256s ", buf[0], buf[1], buf[2], buf[3]);
	// header line, ignore
	if (n > 0 && !strcmp(buf[0], "seg"))
		return ret;
	// invalid line
	if (n != 4)
		return ret;

	ret.segment = atoi(buf[0]);
	//a  0        fffffffe            3612     trap_Error
	//b  0               0               0     vmMain
	//b  0             37e             aad     G_UpdateCvars
	//c  0                           5     LINE 171
	//d  0                           0     LINE 0
	if (ret.segment == 0) {
		// c or d
		if (!strcmp(buf[2], "LINE")) {
			// todo: fix line?
			ret.segment = -1;
			return ret;
			// d
			if (atoi(buf[3]) == 0) {
				ret.segment = -1;
				return ret;
			}
			// c
			else {
				strncatz(buf[2], " ", sizeof(buf[2]));
				strncatz(buf[2], buf[3], sizeof(buf[2]));
				ret.offset = strtoul(buf[1], NULL, 16);
				ret.symbol = _strdup(buf[2]);
			}
		}
		// a or b
		else {
			int i = strtol(buf[1], NULL, 16);
			// a
			if (i < 0) {
				ret.offset = i;
				ret.symbol = _strdup(buf[3]);
			}
			// b
			else {
				ret.offset = strtoul(buf[1], NULL, 16);
				ret.symbol = _strdup(buf[3]);
			}
		}
	}
	//   1             46c               0     gameCvarTableSize
	//   1             4a0            3612     Max_Ammo
	//   2          178d74               0     _stackStart
	//   2          188d74               0     _stackEnd
	//   3               0            3612     g_intermissionTime
	//   3             110            3612     g_podiumDrop
	// segment 2 is where all string literals live, as well as any global/static char array that is initialized, e.g.:
	//     static char ctfFlagStatusRemap[] = { '0', '1', '*', '*', '2' };
	//     char bg_availableOutfitting[WP_NUM_WEAPONS] = {-1};
	// sometimes _stackStart and _stackEnd are put in segment 2 in the .map but they really live in segment 3 (bss)
	else if (ret.segment >= 1 || ret.segment <= 3) {
		if (ret.segment == 2 && !strncmp(buf[3], "_stack", 6))
			ret.segment = 3;
		ret.offset = strtoul(buf[1], NULL, 16);
		ret.symbol = _strdup(buf[3]);
	}
	else {
		ret.segment = -1;
	}

	return ret;
}


static symbolmap_t parse_map_line(char* line) {
	char buf[4][256] = { 0, };
	symbolmap_t ret = {
		-1,		// index
		-1,		// segment
		0,		// offset
		NULL,	// symbol
	};
	int n = sscanf(line, " %256s %256s %256s %256s ", buf[0], buf[1], buf[2], buf[3]);
	// this is a line from a stvoymp q3asm map file
	if (n > 3)
		return parse_map_line_ex(line);
	// invalid line
	if (n != 3)
		return ret;

	ret.segment = atoi(buf[0]);
	ret.offset = strtoul(buf[1], NULL, 16);
	ret.symbol = _strdup(buf[2]);
	return ret;
}
