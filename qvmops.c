/*
QVMOPS - Quake3 Virtual Machine Opcodes disassembler
Copyright 2004-2026
https://github.com/thecybermind/qvmops/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < k.m.masterson@gmail.com >

*/

#define QVMOPS_VERSION "1.1.2"

#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qvm.h"
#include "symbols.h"
#include "util.h"
#include "qvmops.h"


static int process(const char* file);


int main(int argc, char* argv[]) {
	char qvmfile[1024];
	char mapfile[1024];
	char outfile[1024];
	int n = 0;
	int ret = 0;			

	printf("qvmops v" QVMOPS_VERSION "\n\n");
	
	// require a filename parameter
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <file> [mapfile]\n", argv[0]);
		return 1;
	}

	strncpyz(qvmfile, argv[1], sizeof(qvmfile));

	// if no map filename given, look for qvm filename with .map extension
	if (argc == 2) {
		strncpyz(mapfile, argv[1], sizeof(mapfile));
		// look for ".qvm"
		char* p = strrstr(mapfile, ".qvm");
		// if found
		if (p)
			// change to ".map"
			memcpy(p, ".map", 4);
		// otherwise, append ".map"
		else
			strncatz(mapfile, ".map", sizeof(mapfile));
	}
	// otherwise use provided map filename
	else
		strncpyz(mapfile, argv[2], sizeof(mapfile));

	// try to load map file
	parse_map(mapfile);

	// try to load qvm file
	if (!parse_qvm(qvmfile)) {
		fprintf(stderr, "Failed to read QVM file %s", argv[1]);
		return 1;
	}

	// open output file for writing
	strncpyz(outfile, argv[1], sizeof outfile);
	strncatz(outfile, ".txt", sizeof(outfile));
	printf("Processing output file %s...\n", outfile);
	process(outfile);

	// cleanup
	printf("%s written\n", outfile);

	return ret;
}


// output header
static void process_header(FILE* h) {
	puts("Processing header...");
	// output header info
	fputs("HEADER\n======\n", h);
	fprintf(h, "MAGIC: %X\n", header.magic);
	fprintf(h, "OPCOUNT: 0x%X (%i)\n", header.opcount, header.opcount);
	fprintf(h, "CODEOFF: 0x%X (%i)\n", header.codeoffset, header.codeoffset);
	fprintf(h, "CODELEN: 0x%X (%i)\n", header.codelength, header.codelength);
	fprintf(h, "DATAOFF: 0x%X (%i)\n", header.dataoffset, header.dataoffset);
	fprintf(h, "DATALEN: 0x%X (%i)\n", header.datalen, header.datalen);
	fprintf(h, "LITLEN : 0x%X (%i)\n", header.litlen, header.litlen);
	fprintf(h, "BSSLEN : 0x%X (%i)\n", header.bsslen, header.bsslen);
}


// output code segment
static int process_code(FILE* h) {
	int last_enter_index = -1;

	puts("Processing code segment...");
	fputs("\n\nCODE SEGMENT\n============\n", h);
	fputs(" INDEX OFFSETX/OFFSET INSTR     PARAM\n", h);

	// output code info
	for (int index = 0; index < instructioncount; index++) {
		instruction_t* instr = &instructions[index];

		fprintf(h, "%06d %07x/%06d %-9s", index, instr->offset, instr->offset, opcodename(instr->opcode));

		if (opcodeparamsize(instr->opcode))
			fprintf(h, " %-10d", instr->param);
		else
			fputs("           ", h);

		switch (instr->opcode) {
		case OP_ENTER: {
			symbolmap_t* symbol;
			last_enter_index = index;
			symbol = find_code_symbol(index, -1);
			if (symbol)
				fputs(" ;", h);
			else
				fprintf(h, " ; START func%d", index);
			while (symbol) {
				fprintf(h, " START %s", symbol->symbol);
				symbol = find_code_symbol(index, symbol->index);
			}
			break;
		}
		case OP_LEAVE: {
			symbolmap_t* symbol;
			if (last_enter_index < 0)
				break;
			symbol = find_code_symbol(last_enter_index, -1);
			if (symbol)
				fputs(" ;", h);
			else
				fprintf(h, " ; END func%d", last_enter_index);
			while (symbol) {
				fprintf(h, " END %s", symbol->symbol);
				symbol = find_code_symbol(last_enter_index, symbol->index);
			}
			break;
		}
		case OP_CALL: {
			instruction_t* prev_instr;
			symbolmap_t* symbol;
			if (index == 0)
				break;
			prev_instr = &instructions[index - 1];
			if (prev_instr->opcode != OP_CONST)
				break;
			if (prev_instr->param >= instructioncount)
				break;
			symbol = find_code_symbol(prev_instr->param, -1);
			if (symbol)
				fputs(" ;", h);
			else {
				if (prev_instr->param < 0)
					fprintf(h, " ; > trap%d", -prev_instr->param - 1);
				else
					fprintf(h, " ; > func%d", prev_instr->param);
			}
			while (symbol) {
				fprintf(h, " > %s", symbol->symbol);
				symbol = find_code_symbol(prev_instr->param, symbol->index);
			}
			break;
		}
		case OP_JUMP: {
			instruction_t* prev_instr;
			instruction_t* next_instr;
			symbolmap_t* symbol;
			if (index == 0)
				break;
			prev_instr = &instructions[index - 1];
			if (prev_instr->opcode != OP_CONST)
				break;
			if (prev_instr->param >= instructioncount)
				break;
			symbol = find_code_symbol(prev_instr->param, -1);
			if (symbol)
				fputs(" ;", h);
			else {
				for (int i = prev_instr->param; i >= 0; i--) {
					if (instructions[i].opcode == OP_ENTER) {
						fprintf(h, " ; > func%d+%d", i, prev_instr->param - i);
						break;
					}
				}
			}
			while (symbol) {
				fprintf(h, " > %s+%d", symbol->symbol, prev_instr->param - symbol->offset);
				next_instr = &instructions[prev_instr->param + 1];
				if (next_instr->opcode == OP_LEAVE)
					fputs(" (return)", h);
				symbol = find_code_symbol(prev_instr->param, symbol->index);
			}
			break;
		}
		case OP_EQ:
		case OP_NE:
		case OP_LTI:
		case OP_LEI:
		case OP_GTI:
		case OP_GEI:
		case OP_LTU:
		case OP_LEU:
		case OP_GTU:
		case OP_GEU:
		case OP_EQF:
		case OP_NEF:
		case OP_LTF:
		case OP_LEF:
		case OP_GTF:
		case OP_GEF: {
			instruction_t* next_instr;
			symbolmap_t* symbol;
			if (index == 0)
				break;
			symbol = find_code_symbol(instr->param, -1);
			if (symbol)
				fputs(" ;", h);
			else {
				for (int i = instr->param; i >= 0; i--) {
					if (instructions[i].opcode == OP_ENTER) {
						fprintf(h, " ; > func%d+%d", i, instr->param - i);
						break;
					}
				}
			}
			while (symbol) {
				fprintf(h, " > %s+%d", symbol->symbol, instr->param - symbol->offset);
				next_instr = &instructions[instr->param + 1];
				if (next_instr->opcode == OP_LEAVE)
					fputs(" (return)", h);
				symbol = find_code_symbol(instr->param, symbol->index);
			}
			break;
		}
		case OP_CONST: {
			instruction_t* next_instr;
			symbolmap_t* symbol;
			// ignore small literals, not likely memory accesses or jumps
			if (instr->param < 1025)
				break;
			if (instr->param > instructioncount && instr->param > datasize[SEGMENT_DATA] + datasize[SEGMENT_LIT] + datasize[SEGMENT_BSS])
				break;
			if (index == instructioncount - 1)
				break;
			next_instr = &instructions[index + 1];
			if (next_instr->opcode == OP_LOAD1 ||
				next_instr->opcode == OP_LOAD2 ||
				next_instr->opcode == OP_LOAD4) {
				fprintf(h, " ; (%x)", instr->param);
				break;
			}
			if (next_instr->opcode == OP_CALL ||
				next_instr->opcode == OP_JUMP)
				break;
			symbol = find_data_symbol(instr->param, -1);
			if (symbol)
				fputs(" ;", h);
			while (symbol) {
				fprintf(h, " %s+%d (?)", symbol->symbol, instr->param - symbol->offset);
				symbol = find_data_symbol(instr->param, symbol->index);
			}
			break;
		}
		case OP_LOAD1:
		case OP_LOAD2:
		case OP_LOAD4: {
			symbolmap_t* symbol;
			if (index == 0)
				break;
			instruction_t* prev_instr = &instructions[index - 1];
			if (prev_instr->opcode != OP_CONST)
				break;
			symbol = find_data_symbol(prev_instr->param, -1);
			if (symbol)
				fputs(" ;", h);
			while (symbol) {
				fprintf(h, " %s+%d", symbol->symbol, prev_instr->param - symbol->offset);
				symbol = find_data_symbol(prev_instr->param, symbol->index);
			}
			break;
		}
		default:
			;
		}

		fputs("\n", h);
		fflush(h);
	}
}


static void process_data(FILE* h) {
	if (symbolcount[SEGMENT_DATA] + symbolcount[SEGMENT_LIT] + symbolcount[SEGMENT_BSS] == 0)
		return;

	puts("Processing data segment symbols...");
}


static void process_data_hex(FILE* h) {
	uint8_t* p;
	puts("Processing data segment hex view...");
	fputs("\n\nDATA SEGMENT\n============\n", h);
	fprintf(h, "LIT segment begins at offset %X (look for | in row %X)\n", datasize[SEGMENT_DATA], datasize[SEGMENT_DATA] & 0xFFFFFFE0);

	// start pointer at start of data segment
	p = data;

	// loop through each byte in data segment
	while (p < data + datasize[SEGMENT_DATA] + datasize[SEGMENT_LIT]) {
		// print offset
		fprintf(h, "%04X ", p - data);

		// print hex values
		for (int b = 0; b < DATA_ROW_LEN; b++) {
			// halfway through the row, print a gap
			if (b == DATA_ROW_LEN / 2)
				fputs("   ", h);
			// if this row runs out of data before the end, print empty spaces
			if (p + b >= data + datasize[SEGMENT_DATA] + datasize[SEGMENT_LIT])
				fputs("   ", h);
			// if this is the split between data and lit, put a bar
			else if (p + b == data + datasize[SEGMENT_DATA])
				fprintf(h, "|%02X", p[b]);
			else
				fprintf(h, " %02X", p[b]);
		}

		fputs("    ", h);

		// print characters
		for (int b = 0; b < DATA_ROW_LEN; b++) {
			// halfway through the row, print a gap
			if (b == DATA_ROW_LEN / 2)
				fprintf(h, " ");
			// if this row runs out of data before the end, print empty spaces
			if (p + b >= data + datasize[SEGMENT_DATA] + datasize[SEGMENT_LIT])
				fprintf(h, " ");
			else
				fprintf(h, "%c", printablec(p[b]));
		}

		fprintf(h, "\n");
		fflush(h);

		p += DATA_ROW_LEN;
	}
}


static int process(const char* file) {
	FILE* h;

	h = fopen(file, "w");
	if (!h || ferror(h)) {
		fprintf(stderr, "File not found: %s\n", file);
		goto fail;
	}

	process_header(h);

	process_code(h);

	process_data(h);

	process_data_hex(h);

	return 1;
fail:
	if (h)
		fclose(h);
	return 0;
}
