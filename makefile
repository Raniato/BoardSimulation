CC=gcc
CFLAGS=-c -g 

all: board boardserver boardpost

board: board.c
	$(CC) -o board board.c 

boardserver: boardserver.o
	$(CC) -o boardserver boardserver.o 

boardserver.o: boardserver.c serveraux.h
	$(CC) $(CFLAGS) boardserver.c

boardpost: boardpost.c
	$(CC) -o boardpost boardpost.c 
