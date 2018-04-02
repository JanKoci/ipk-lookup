/******** args_parser.h - IPK Project 2 **********
* Author  : Jan Koci
* Date    : 24.3.2018
* Brief   : parsing command line arguments for ipk-lookup.c
************************************************/
#include <stdbool.h>
#ifndef ARGS_PARSER
#define ARGS_PARSER

typedef struct {
  bool help;
  bool iterative;
  const char* server;
  const char* type;
  const char* name;
  int timeout;
}Arguments;

void print_usage(const char*);
void print_help(const char*);
void init_args(Arguments*);
void parse(int, char const**, Arguments*);

#endif // ARGS_PARSER
