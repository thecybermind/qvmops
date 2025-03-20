
CC=gcc

qvmops: main.o
	$(CC) -o qvmops main.o
  
main.o: main.c
	$(CC) -o main.o -c main.c
