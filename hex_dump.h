/******** hex_dump.h - IPK Project 2 **********
* Author: Jan Koci
* Date: 24.3.2018
* Brief: tool for printing hex and binary representation of data
************************************************/
#ifndef HEX_DUMP_H
#define HEX_DUMP_H

#include "ipk-lookup.h"

/**
* @brief print struct dns_answer
* @param ans  the structure to print
*/
void print_ans(struct dns_answer* ans);

/**
* @brief print binary representation of char
* @param c    the character to print in binary
*/
void printbinchar(char c);

/**
* @brief print hexa and binary representation of data
* @param data   data to print
* @param size   size of data to print
*/
void hex_dump(unsigned char* data, int size);

#endif // HEX_DUMP_H
