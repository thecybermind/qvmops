/*
QVMOPS - Quake3 Virtual Machine Opcodes disassembler
Copyright 2004-2026
https://github.com/thecybermind/qvmops/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
	Kevin Masterson < k.m.masterson@gmail.com >

*/

#pragma once
#ifndef QVMOPS_QVM_H
#define QVMOPS_QVM_H

#include <stdint.h>

// magic numbers at start of .qvm
// .qvm is generated with little endian order
// magic number appears in file as 44 14 72 12
#define VM_MAGIC	0x12721444	// little endian

// opcodes
typedef enum vmop_e {
	OP_UNDEF,
	OP_NOP,
	OP_BREAK,
	OP_ENTER,
	OP_LEAVE,
	OP_CALL,
	OP_PUSH,
	OP_POP,
	OP_CONST,
	OP_LOCAL,
	OP_JUMP,
	OP_EQ,
	OP_NE,
	OP_LTI,
	OP_LEI,
	OP_GTI,
	OP_GEI,
	OP_LTU,
	OP_LEU,
	OP_GTU,
	OP_GEU,
	OP_EQF,
	OP_NEF,
	OP_LTF,
	OP_LEF,
	OP_GTF,
	OP_GEF,
	OP_LOAD1,
	OP_LOAD2,
	OP_LOAD4,
	OP_STORE1,
	OP_STORE2,
	OP_STORE4,
	OP_ARG,
	OP_BLOCK_COPY,
	OP_SEX8,
	OP_SEX16,
	OP_NEGI,
	OP_ADD,
	OP_SUB,
	OP_DIVI,
	OP_DIVU,
	OP_MODI,
	OP_MODU,
	OP_MULI,
	OP_MULU,
	OP_BAND,
	OP_BOR,
	OP_BXOR,
	OP_BCOM,
	OP_LSH,
	OP_RSHI,
	OP_RSHU,
	OP_NEGF,
	OP_ADDF,
	OP_SUBF,
	OP_DIVF,
	OP_MULF,
	OP_CVIF,
	OP_CVFI
} vmop_t;

// QVM header
typedef struct vmheader_s {
	int magic;
	int opcount;
	int codeoffset;
	int codelength;
	int dataoffset;
	int datalen;
	int litlen;
	int bsslen;
} vmheader_t;

const char* opcodename(vmop_t op);
int opcodeparamsize(vmop_t op);

// segment numbers
enum {
	SEGMENT_CODE,
	SEGMENT_DATA,
	SEGMENT_LIT,
	SEGMENT_BSS,
	SEGMENT_COUNT,
};

extern vmheader_t header;

#define MAX_INSTRUCTIONS 1000000
// a single instruction in code segment
typedef struct instruction_s {
	int offset;			// byte offset into QVM (to match symbols)
	vmop_t opcode;		// opcode
	int param;			// hardcoded parameter
} instruction_t;
extern instruction_t instructions[MAX_INSTRUCTIONS];
extern int instructioncount;

extern uint8_t* data;
extern int datasize[SEGMENT_COUNT];

// fill instructions array
int parse_qvm(const char* file);


#endif // QVMOPS_QVM_H
