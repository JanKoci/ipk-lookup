# Makefile - IPK Project 2
# Author: Jan Koci
# Date: 21.3.2018
# Brief: Makefile for a school project implementing
# 					a DNS lookup application

CC = gcc
FLAGS = -Wall -Wextra -pedantic -Werror
OBJ = *.o
PROG = ipk-lookup

all: ${PROG}

ipk-lookup.o: ipk-lookup.c
	${CC} ${FLAGS} ipk-lookup.c -c

hex_dump.o: hex_dump.c
	${CC} ${FLAGS} hex_dump.c -c

args_parser.o: args_parser.c
	${CC} ${FLAGS} args_parser.c -c

dns_convert.o: dns_convert.c
	${CC} ${FLAGS} dns_convert.c -c

iterative.o: iterative.c
	${CC} ${FLAGS} iterative.c -c

${PROG}: ipk-lookup.o hex_dump.o args_parser.o dns_convert.o iterative.o
	${CC} ${FLAGS} ipk-lookup.o hex_dump.o args_parser.o dns_convert.o iterative.o -o ${PROG}

zip:
	zip xkocij01.zip *.c *.h Makefile README.md *.pdf

clean:
	rm ${OBJ} ${PROG}
