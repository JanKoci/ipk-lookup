/******** args_parser.h - IPK Project 2 **********
* Author  : Jan Koci
* Date    : 24.3.2018
* Brief   : parsing command line arguments for ipk-lookup.c
************************************************/
#include <stdbool.h>
#ifndef ARGS_PARSER
#define ARGS_PARSER

/**
* @struct Arguments represents command line arguments
* help      -h flag indicator, print help
* iterative -i flag indicator, apply iterative resolution
* server    -s servername, server to send queries to
* type      -t typename, type of the query
* name      domain name or IP address to query about
* timeout   -T timeout, timeout for the query
*/
typedef struct {
  bool help;
  bool iterative;
  const char* server;
  const char* type;
  const char* name;
  int timeout;
}Arguments;

/**
* @brief print program usage
* @param program  name of the program (argv[0])
*/
void print_usage(const char*);

/**
* @brief print detail usage with all command line arguments explained
* @param program  name of the program (argv[0])
*/
void print_help(const char*);

/**
* @brief initialize Arguments structure
* @param args   the Arguments structure to initialize
*/
void init_args(Arguments*);

/**
* @brief parse command line arguments
* @param argc   program's argc (number of argumets passed)
* @param argv   program's argv (vector containing all arguments passed)
* @param args   store here parsed arguments
*/
void parse(int, char const**, Arguments*);

#endif // ARGS_PARSER
