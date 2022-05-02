SHELL = /bin/bash
CC = gcc
C+ = g++
CFLAGS = -pthread -g
SRC = $(wildcard *.cpp) $(wildcard *.c)
EXE = $(subst .,,$(SRC))
all: ${EXE}

%cpp: %.cpp
	@${C+} ${CFLAGS} $*.cpp -o $*cpp-ron
%c:	%.c
	@${CC} ${CFLAGS} $*.c -o $*c-ron

clean:
	@rm *-ron