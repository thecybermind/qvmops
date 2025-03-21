/*
QVMOPS - Quake3 Virtual Machine Opcodes
Copyright 2004-2024
https://github.com/thecybermind/qvmops/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
    Kevin Masterson < cybermind@gmail.com >

*/

#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//magic numbers at start of .qvm
//.qvm is generated with little endian order
//magic number appears in file as 44 14 72 12
#define	VM_MAGIC	0x12721444	//little endian

#define DATA_ROW_LEN	32

//opcodes
typedef enum {
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
} vmops_t;

//return a padded string for the opcode name
const char* opcodename(vmops_t op) {
	switch(op) {
	case OP_UNDEF:
		return "OP_UNDEF ";
	case OP_NOP:
		return "OP_NOP   ";
	case OP_BREAK:
		return "OP_BREAK ";
	case OP_ENTER:
		return "OP_ENTER ";
	case OP_LEAVE:
		return "OP_LEAVE ";
	case OP_CALL:
		return "OP_CALL  ";
	case OP_PUSH:
		return "OP_PUSH  ";
	case OP_POP:
		return "OP_POP   ";
	case OP_CONST:
		return "OP_CONST ";
	case OP_LOCAL:
		return "OP_LOCAL ";
	case OP_JUMP:
		return "OP_JUMP  ";
	case OP_EQ:
		return "OP_EQ    ";
	case OP_NE:
		return "OP_NE    ";
	case OP_LTI:
		return "OP_LTI   ";
	case OP_LEI:
		return "OP_LEI   ";
	case OP_GTI:
		return "OP_GTI   ";
	case OP_GEI:
		return "OP_GEI   ";
	case OP_LTU:
		return "OP_LTU   ";
	case OP_LEU:
		return "OP_LEU   ";
	case OP_GTU:
		return "OP_GTU   ";
	case OP_GEU:
		return "OP_GEU   ";
	case OP_EQF:
		return "OP_EQF   ";
	case OP_NEF:
		return "OP_NEF   ";
	case OP_LTF:
		return "OP_LTF   ";
	case OP_LEF:
		return "OP_LEF   ";
	case OP_GTF:
		return "OP_GTF   ";
	case OP_GEF:
		return "OP_GEF   ";
	case OP_LOAD1:
		return "OP_LOAD1 ";
	case OP_LOAD2:
		return "OP_LOAD2 ";
	case OP_LOAD4:
		return "OP_LOAD4 ";
	case OP_STORE1:
		return "OP_STORE1";
	case OP_STORE2:
		return "OP_STORE2";
	case OP_STORE4:
		return "OP_STORE4";
	case OP_ARG:
		return "OP_ARG   ";
	case OP_BLOCK_COPY:
		return "OP_BLKCPY";
	case OP_SEX8:
		return "OP_SEX8  ";
	case OP_SEX16:
		return "OP_SEX16 ";
	case OP_NEGI:
		return "OP_NEGI  ";
	case OP_ADD:
		return "OP_ADD   ";
	case OP_SUB:
		return "OP_SUB   ";
	case OP_DIVI:
		return "OP_DIVI  ";
	case OP_DIVU:
		return "OP_DIVU  ";
	case OP_MODI:
		return "OP_MODI  ";
	case OP_MODU:
		return "OP_MODU  ";
	case OP_MULI:
		return "OP_MULI  ";
	case OP_MULU:
		return "OP_MULU  ";
	case OP_BAND:
		return "OP_BAND  ";
	case OP_BOR:
		return "OP_BOR   ";
	case OP_BXOR:
		return "OP_BXOR  ";
	case OP_BCOM:
		return "OP_BCOM  ";
	case OP_LSH:
		return "OP_LSH   ";
	case OP_RSHI:
		return "OP_RSHI  ";
	case OP_RSHU:
		return "OP_RSHU  ";
	case OP_NEGF:
		return "OP_NEGF  ";
	case OP_ADDF:
		return "OP_ADDF  ";
	case OP_SUBF:
		return "OP_SUBF  ";
	case OP_DIVF:
		return "OP_DIVF  ";
	case OP_MULF:
		return "OP_MULF  ";
	case OP_CVIF:
		return "OP_CVIF  ";
	case OP_CVFI:
		return "OP_CVFI  ";
	default:
		return "unknown  ";
	}

	return "unknown  ";
}

//QVM header
typedef struct {
	int magic;
	int opcount;
	int codeoffset;
	int codelength;
	int dataoffset;
	int datalen;
	int litlen;
	int bsslen;
} vmheader_t;

//"byte" is cooler than "char" any day of the week
typedef unsigned char byte;

char printc(byte x) {
	if (x < 32 || x == 127 || x == 255)
		return '.';
	return *(char*)&x;
}

int main(int argc, char* argv[]) {
	char outfile[1024];	//output file name, basically the qvm name with .txt stuck on the end
	int qvmsize = 0;	//size of the qvm file
	int n = 0;		//temp int
	FILE* htxt;		//stream for the output file
	FILE* hqvm;		//stream for the input file
	byte* qvm;		//block to store whole file in memory
	byte* p;		//start of code segment
	vmheader_t* header;	//same as 'qvm' just used to interpret it a bit differently :)

	//require a filename parameter
	if (argc < 2) {
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
	}

	//open the .qvm file for reading
	hqvm = fopen(argv[1], "rb");

	//error checking is your friend
	if (!hqvm || feof(hqvm)) {
		printf("File not found: %s\n", argv[1]);
		return 1;
	}


	//grab file size
	n = ftell(hqvm);
	fseek(hqvm, 0, SEEK_END);
	qvmsize = ftell(hqvm);
	fseek(hqvm, n, SEEK_SET) ;

	//allocate enough memory for the whole thing
	qvm = (byte*)malloc(qvmsize);

	//this should only happen if your comp sucks
	if (!qvm) {
		printf("Unable to allocate memory block: %d\n", qvmsize);
		return 1;
	}

	//read the file into memory and close the file handle
	fread(qvm, 1, qvmsize, hqvm);
	fclose(hqvm);

	header = (vmheader_t*)qvm;

	//if the magic number doesn't match, abort
	if (header->magic != VM_MAGIC) {
		printf("Invalid QVM file, aborting\n");
		exit(1);
	}

	//if the header has false code segment data, abort
	if (header->codeoffset > qvmsize || header->codeoffset + header->codelength > qvmsize) {
		printf("Invalid QVM file, aborting\n");
		exit(1);
	}

	//open output file for writing
	strncpy(outfile, argv[1], sizeof outfile);
	outfile[sizeof(outfile) - 1] = '\0';
	strncat(outfile, ".txt", sizeof(outfile) - strlen(outfile) - 1);
	htxt = fopen(outfile, "w");

	//output header info
	fprintf(htxt, "HEADER\n======\n");
	fprintf(htxt, "MAGIC: %X\n", header->magic);
	fprintf(htxt, "OPCOUNT: %i\n", header->opcount);
	fprintf(htxt, "CODEOFF: 0x%X (%i)\n", header->codeoffset, header->codeoffset);
	fprintf(htxt, "CODELEN: 0x%X (%i)\n", header->codelength, header->codelength);
	fprintf(htxt, "DATAOFF: 0x%X (%i)\n", header->dataoffset, header->dataoffset);
	fprintf(htxt, "DATALEN: 0x%X (%i)\n", header->datalen, header->datalen);
	fprintf(htxt, "LITLEN : 0x%X (%i)\n", header->litlen, header->litlen);
	fprintf(htxt, "BSSLEN : 0x%X (%i)\n", header->bsslen, header->bsslen);
	
	fprintf(htxt, "\n\nCODE SEGMENT\n============\n");

	//start pointer at start of code segment
	p = qvm + header->codeoffset;

	//loop through each instruction
	for (n = 0; n < header->opcount && p < qvm + header->codeoffset + header->codelength; ++n) {
		int op = *p;

		// output offset
		fprintf(htxt, "%06d ", n);

		switch (op) {
			//4 byte arg ops
			case OP_ENTER:
			case OP_LEAVE:
			case OP_CONST:
			case OP_LOCAL:
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
			case OP_BLOCK_COPY:
				++p;
				fprintf(htxt, "%s %d\n", opcodename(op), *(int*)p);
				p += 4;
				break;
			//1 byte arg ops
			case OP_ARG:
				++p;
				fprintf(htxt, "%s %d\n", opcodename(op), *p);
				++p;
				break;
			//no arg op
			default:
				fprintf(htxt, "%s\n", opcodename(op));
				++p;
				break;
		}
	}

	fprintf(htxt, "\n\nDATA SEGMENT\n============\n");

	//start pointer at start of data segment
	p = qvm + header->dataoffset;

	//loop through each byte in data segment
	while (p < qvm + header->dataoffset + header->datalen + header->litlen) {
		// print offset
		fprintf(htxt, "%04X ", p - qvm - header->dataoffset);

		// print hex values
		for (int b = 0; b < DATA_ROW_LEN; b++) {
			// halfway through the row, print a gap
			if (b == DATA_ROW_LEN / 2)
				fprintf(htxt, "   ");
			// if this row runs out of data before the end, print empty spaces
			if (p + b >= qvm + header->dataoffset + header->datalen + header->litlen)
				fprintf(htxt, "   ");
			// if this is the split between data and lit, put a bar
			else if (p + b == qvm + header->dataoffset + header->datalen)
				fprintf(htxt, "|%02X", p[b]);
			else
				fprintf(htxt, " %02X", p[b]);
		}

		fprintf(htxt, "    ");

		// print characters
		for (int b = 0; b < DATA_ROW_LEN; b++) {
			// halfway through the row, print a gap
			if (b == DATA_ROW_LEN / 2)
				fprintf(htxt, " ");
			// if this row runs out of data before the end, print empty spaces
			if (p + b >= qvm + header->dataoffset + header->datalen + header->litlen)
				fprintf(htxt, " ");
			else
				fprintf(htxt, "%c", printc(p[b]));
		}

		fprintf(htxt, "\n");

		p += DATA_ROW_LEN;
	}

	//close up shop and go fishing
	free(qvm);
	fclose(htxt);
	printf("%s written\n", outfile);

	return 0;
}
