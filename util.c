/*
QVMOPS - Quake3 Virtual Machine Opcodes disassembler
Copyright 2004-2026
https://github.com/thecybermind/qvmops/
3-clause BSD license: https://opensource.org/license/bsd-3-clause

Created By:
	Kevin Masterson < k.m.masterson@gmail.com >

*/

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"


// "safe" strncpy that always null-terminates
char* strncpyz(char* dest, const char* src, size_t count) {
    char* ret = strncpy(dest, src, count);
    dest[count - 1] = '\0';
    return ret;
}


// "safe" strncat where count is the total size of dest, and always null-terminates WITHIN the buffer
char* strncatz(char* dest, const char* src, size_t count) {
	size_t len = strlen(dest);
	if (len >= count)
		return dest;
	// free space available in dest
	size_t avail = count - len;
	// strncat may null terminate at count+1 which may be outside the buffer if count is the full buffer size
	if (avail)
		avail--;
	char* ret = strncat(dest, src, avail);
	dest[count - 1] = '\0';
	return ret;
}


// return a printable character for c ('.' for unprintable)
char printablec(uint8_t c) {
	if (c < 32 || c >= 127)
		return '.';
	return *(char*)&c;
}


// reverse version of strstr (search backwards from end of string)
char* strrstr(const char* str, const char* substr) {
	char* prev = NULL;
	char* current = strstr(str, substr);
	while (current) {
		prev = current;
		current = strstr(current + 1, substr);
	}
	return prev;
}


#ifdef _MSC_VER
// https://stackoverflow.com/questions/735126/a/47229318#47229318
ssize_t getline(char** lineptr, size_t* n, FILE* stream) {
	size_t pos;
	int c;

	if (lineptr == NULL || stream == NULL || n == NULL) {
		errno = EINVAL;
		return -1;
	}

	c = getc(stream);
	if (c == EOF) {
		return -1;
	}

	if (*lineptr == NULL) {
		*lineptr = malloc(MINIMUM_BUFFER_SIZE);
		if (*lineptr == NULL) {
			return -1;
		}
		*n = MINIMUM_BUFFER_SIZE;
	}

	pos = 0;
	while (c != EOF) {
		if (pos + 1 >= *n) {
			size_t new_size = *n + (*n >> 2);

			// have some reasonable minimum
			if (new_size < MINIMUM_BUFFER_SIZE) {
				new_size = MINIMUM_BUFFER_SIZE;
			}

			// size_t wraparound
			if (new_size <= *n) {
				errno = ENOMEM;
				return -1;
			}

			// Note you might also want to check that PTRDIFF_MAX
			// is not exceeded!

			char* new_ptr = realloc(*lineptr, new_size);
			if (new_ptr == NULL) {
				return -1;
			}
			*n = new_size;
			*lineptr = new_ptr;
		}

		((unsigned char*)(*lineptr))[pos++] = c;
		if (c == '\n') {
			break;
		}
		c = getc(stream);
	}

	(*lineptr)[pos] = '\0';

	// if an IO error occurred, return -1
	if (c == EOF && !feof(stream)) {
		return -1;
	}

	// otherwise we successfully read until the end-of-file
	// or the delimiter
	return pos;
}
#endif
