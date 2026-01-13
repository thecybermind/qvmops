
CC=gcc

SRC_C   := $(wildcard *.c)
OBJ     := $(SRC_C:%.c=%.o)

qvmops: $(OBJ)
	$(CC) -m32 -o qvmops $(OBJ)
  
%.o: %.c
	$(CC) -m32 -o $@ -c $<
