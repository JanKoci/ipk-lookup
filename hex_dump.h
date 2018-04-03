/******** hex_dump.h - IPK Project 2 **********
* Author: Jan Koci
* Date: 24.3.2018
* Brief: tool for printing hex and binary representation of bytes
************************************************/
#ifndef HEX_DUMP_H
#define HEX_DUMP_H

#include "ipk-lookup.h"

void print_ans(struct dns_answer* ans);
void printbinchar(char c);
void hex_dump(char* data, int size);

#endif // HEX_DUMP_H
