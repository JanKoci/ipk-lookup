/******** hex_dump.c - IPK Project 2 **********
* Author: Jan Koci
* Date: 24.3.2018
* Brief: tool for printing hex and binary representation of bytes
************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "hex_dump.h"
// #include "ipk-lookup.h"


void print_ans(struct dns_answer* ans)
{
  if (ans->cls == 1)
  {
    printf("IN ");
  }
  else
  {
    printf("cls = %hu ", ans->cls);
  }
  switch (ans->type) {
    case 1:
      printf("A ");
      break;
    case 2:
      printf("NS ");
      break;
    case 5:
      printf("CNAME ");
      break;
    case 12:
      printf("PTR ");
      break;
    case 28:
      printf("AAAA ");
      break;
    default:
      printf("fuck = %hu ", ans->type);
  }
  // debug
  // printf("ttl = %u ", ans->ttl);
  // printf("data_len = %hu ", ans->data_len);
}

void printbinchar(char c)
{
    for (int i = 7; i >= 0; --i)
    {
        putchar( (c & (1 << i)) ? '1' : '0' );
    }
}

void hex_dump(unsigned char* data, int size)
{
  char previous;
  for (int i = 0; i < size; i++)
  {
    if (((i+1)%2) == 0)
    {
      printf("%02x  ", data[i]);
      printbinchar(previous);
      putchar(' ');
      printbinchar(data[i]);
      putchar('\n');
    }
    else {
      printf("%02x ", data[i]);
      previous = data[i];
      if (i == (size-1)) {
        printf("    ");
        printbinchar(data[i]);
        putchar('\n');
      }
    }
  }
}
