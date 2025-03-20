qvmops is a tool that functions like a very basic disassembler for compiled .qvm files.

Usage: qvmops <.qvm file>

The output will be in a text file with the same name as the .qvm file with ".txt" appended.

The output will simply be header info followed a linear list of all opcodes, including 1- or 4-byte data if available, and finally followed by a hex-editor-style view of the data segment.
