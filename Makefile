CC=gcc 
CFLAGS=-Wall -g

all: skshell
program: skshell.o
program.o: skshell.c 

clean:
	rm -f skshell skshell.o
run: skshell
	./skshell
