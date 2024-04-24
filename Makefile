
CFLAGS = -W -Wall
CC = gcc
OBJS= main.o string_parser.o command.o

all: pseudo-shell.exe

pseudo-shell.exe: command.o string_parser.o main.o
	$(CC) -o pseudo-shell.exe command.o string_parser.o main.o

main.o: main.c command.h string_parser.h
	$(CC) $(CFLAGS) -c main.c

command.o: command.c command.h
	$(CC) $(CFLAGS) -c command.c

string_parser.o: string_parser.c string_parser.h
	$(CC) $(CFLAGS) -c string_parser.c

clean:
	rm -f pseudo-shell.exe

clean_all:
	rm -f pseudo-shell.exe main.o string_parser.o command.o