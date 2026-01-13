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
#include <stdint.h>
#include <malloc.h>
#include "qvm.h"

vmheader_t header;

instruction_t instructions[MAX_INSTRUCTIONS];
int instructioncount;

uint8_t* data[SEGMENT_COUNT];
int datasize[SEGMENT_COUNT];


int parse_qvm(const char* file) {
	FILE* h;
	uint8_t* qvm;
	int qvmsize;
	uint8_t* p;
	int n;

	printf("Opening %s...\n", file);

	h = fopen(file, "rb");
	if (!h || feof(h) || ferror(h)) {
		fprintf(stderr, "File not found: %s\n", file);
		return 0;
	}

	// grab qvm file size
	n = ftell(h);
	fseek(h, 0, SEEK_END);
	qvmsize = ftell(h);
	fseek(h, n, SEEK_SET);

	// allocate enough memory for the whole thing
	qvm = (uint8_t*)malloc(qvmsize);
	if (!qvm) {
		fprintf(stderr, "Unable to allocate qvm memory block: %d\n", qvmsize);
		goto fail;
	}

	// read the file into memory and close file
	fread(qvm, 1, qvmsize, h);
	fclose(h);
	h = NULL;

	memcpy(&header, qvm, sizeof(vmheader_t));

	// if the magic number doesn't match, abort
	if (header.magic != VM_MAGIC) {
		fprintf(stderr, "Invalid QVM file: magic number mismatch\n");
		goto fail;
	}

	// if the segment lengths doesn't match the file size
	if (qvmsize != sizeof(vmheader_t) + header.codelength + header.datalen + header.litlen) {
		fprintf(stderr, "Invalid QVM file: file size doesn't match segment lengths\n");
		goto fail;
	}

	// if the header has false code segment info, abort
	if (header.codeoffset < sizeof(vmheader_t) || header.codeoffset > qvmsize || header.codeoffset + header.codelength > qvmsize) {
		fprintf(stderr, "Invalid QVM file: invalid code offset/length\n");
		goto fail;
	}

	// if the header has false data segment info, abort
	if (header.dataoffset < sizeof(vmheader_t) || header.dataoffset > qvmsize || header.dataoffset + header.datalen + header.litlen > qvmsize) {
		fprintf(stderr, "Invalid QVM file: invalid data offset/length\n");
		goto fail;
	}

	// start pointer at start of code segment
	p = qvm + header.codeoffset;

	int op;

	// loop through each instruction in qvm file
	for (int index = 0; index < header.opcount && p < qvm + header.codeoffset + header.codelength; ++index, ++instructioncount) {
		op = *p;

		instructions[index].offset = p - (qvm + header.codeoffset);
		instructions[index].opcode = op;

		++p;

		n = opcodeparamsize(op);
		if (n == 1)
			instructions[index].param = (int)*p;
		else if (n == 4)
			instructions[index].param = *(int*)p;
		else
			instructions[index].param = 0;

		p += n;
	}

	if (instructioncount != header.opcount) {
		fprintf(stderr, "Invalid QVM file: couldn't read %d instructions\n", header.opcount);
		goto fail;
	}

	// copy data segments for later examination
	datasize[SEGMENT_DATA] = header.datalen;
	data[SEGMENT_DATA] = malloc(datasize[SEGMENT_DATA]);

	datasize[SEGMENT_LIT] = header.litlen;
	data[SEGMENT_LIT] = malloc(datasize[SEGMENT_LIT]);

	return 1;
fail:
	free(qvm);
	if (h)
		fclose(h);
	return 0;
}


// return a padded string for the opcode name
const char* opcodename(vmop_t op) {
	switch (op) {
	case OP_UNDEF:
		return "OP_UNDEF";
	case OP_NOP:
		return "OP_NOP";
	case OP_BREAK:
		return "OP_BREAK";
	case OP_ENTER:
		return "OP_ENTER";
	case OP_LEAVE:
		return "OP_LEAVE";
	case OP_CALL:
		return "OP_CALL";
	case OP_PUSH:
		return "OP_PUSH";
	case OP_POP:
		return "OP_POP";
	case OP_CONST:
		return "OP_CONST";
	case OP_LOCAL:
		return "OP_LOCAL";
	case OP_JUMP:
		return "OP_JUMP";
	case OP_EQ:
		return "OP_EQ";
	case OP_NE:
		return "OP_NE";
	case OP_LTI:
		return "OP_LTI";
	case OP_LEI:
		return "OP_LEI";
	case OP_GTI:
		return "OP_GTI";
	case OP_GEI:
		return "OP_GEI";
	case OP_LTU:
		return "OP_LTU";
	case OP_LEU:
		return "OP_LEU";
	case OP_GTU:
		return "OP_GTU";
	case OP_GEU:
		return "OP_GEU";
	case OP_EQF:
		return "OP_EQF";
	case OP_NEF:
		return "OP_NEF";
	case OP_LTF:
		return "OP_LTF";
	case OP_LEF:
		return "OP_LEF";
	case OP_GTF:
		return "OP_GTF";
	case OP_GEF:
		return "OP_GEF";
	case OP_LOAD1:
		return "OP_LOAD1";
	case OP_LOAD2:
		return "OP_LOAD2";
	case OP_LOAD4:
		return "OP_LOAD4";
	case OP_STORE1:
		return "OP_STORE1";
	case OP_STORE2:
		return "OP_STORE2";
	case OP_STORE4:
		return "OP_STORE4";
	case OP_ARG:
		return "OP_ARG";
	case OP_BLOCK_COPY:
		return "OP_BLKCPY";
	case OP_SEX8:
		return "OP_SEX8";
	case OP_SEX16:
		return "OP_SEX16";
	case OP_NEGI:
		return "OP_NEGI";
	case OP_ADD:
		return "OP_ADD";
	case OP_SUB:
		return "OP_SUB";
	case OP_DIVI:
		return "OP_DIVI";
	case OP_DIVU:
		return "OP_DIVU";
	case OP_MODI:
		return "OP_MODI";
	case OP_MODU:
		return "OP_MODU";
	case OP_MULI:
		return "OP_MULI";
	case OP_MULU:
		return "OP_MULU";
	case OP_BAND:
		return "OP_BAND";
	case OP_BOR:
		return "OP_BOR";
	case OP_BXOR:
		return "OP_BXOR";
	case OP_BCOM:
		return "OP_BCOM";
	case OP_LSH:
		return "OP_LSH";
	case OP_RSHI:
		return "OP_RSHI";
	case OP_RSHU:
		return "OP_RSHU";
	case OP_NEGF:
		return "OP_NEGF";
	case OP_ADDF:
		return "OP_ADDF";
	case OP_SUBF:
		return "OP_SUBF";
	case OP_DIVF:
		return "OP_DIVF";
	case OP_MULF:
		return "OP_MULF";
	case OP_CVIF:
		return "OP_CVIF";
	case OP_CVFI:
		return "OP_CVFI";
	default:
		return "unknown";
	}

	return "unknown";
}


// return size of param for opcode
int opcodeparamsize(vmop_t op) {
	switch (op) {
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
	case OP_GEF:
	case OP_ENTER:
	case OP_LEAVE:
	case OP_CONST:
	case OP_LOCAL:
	case OP_BLOCK_COPY:
		return 4;
	case OP_ARG:
		return 1;
	default:
		return 0;
	}
}
