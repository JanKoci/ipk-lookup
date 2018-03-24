# Makefile - IPK Project 2
# Author: Jan Koci
# Date: 21.3.2018
# Brief: Makefile for a school project implementing
# 					a DNS lookup application

CC = gcc
FLAGS = -Wall -Wextra -pedantic -Werror -g
OBJ = *.o
PROG = ipk-lookup

all: ${PROG}

%.o: %.c %.h
	${CC} ${FLAGS} $< -c

${PROG}: ipk-lookup.o hex_dump.o args_parser.o
	${CC} ${FLAGS} $^ -o $@

clean:
	rm ${OBJ} ${PROG}
