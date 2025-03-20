
CC=gcc

qvmops: main.o
	$(CC) -m32 -o qvmops main.o
  
main.o: main.c
	$(CC) -m32 -o main.o -c main.c
