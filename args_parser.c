/******** args_parser.c - IPK Project 2 **********
* Author  : Jan Koci
* Date    : 24.3.2018
* Brief   : parsing command line arguments for ipk-lookup.c
************************************************/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "args_parser.h"


void init_args(Arguments* args)
{
  args->help = false;
  args->iterative = false;
  args->server = NULL;
  args->type = NULL;
  args->name = NULL;
  args->timeout = 5;
  args->type = "A";

}

void print_usage(const char* program)
{
  printf("USAGE: %s -s server [-T timeout] [-t type] [-i] name\n", program);
}

void print_help(const char* program)
{
  print_usage(program);
  putchar('\n');
  printf("s (server)\t\tDNS server (IPv4 address) to query\n");
  printf("T (timeout)\t\ttimeout for the query, default 5 seconds\n");
  printf("t (type)\t\ttype of the queried record: A (default), AAAA, NS, PTR, CNAME\n");
  printf("i (iterative)\t\tforces iterative queries\n");
  printf("name\t\t\tdomain name to translate or IPv4/IPv6 address if 'type' is PTR\n");
  putchar('\n');
}

void parse(int argc, char const* argv[], Arguments* args)
{
  int opt;
  while ((opt = getopt(argc, (char * const*)argv, "hs:T:t:i")) != -1) {
    switch (opt) {
      case 'h':
        args->help = true;
        break;
      case 's':
        args->server = optarg;
        break;
      case 'T':
        args->timeout = atoi(optarg);
        if (args->timeout <= 0)
        {
          fprintf(stderr, "ERROR: Argument timeout must be grater than 0\n");
          print_usage(argv[0]);
          exit(1);
        }
        break;
      case 't':
        args->type = optarg;
        if ((strcmp(args->type, "A") != 0) && (strcmp(args->type, "AAAA") != 0) \
            && (strcmp(args->type, "NS") != 0) && \
            (strcmp(args->type, "PTR") != 0) && (strcmp(args->type, "CNAME") != 0))
        {
          fprintf(stderr, "ERROR: Invalid type entered: %s\n", args->type);
          print_usage(argv[0]);
          exit(1);
        }
        break;
      case 'i':
        args->iterative = true;
        break;
      case '?':
        fprintf(stderr, "ERROR: Unknown arguments passed\n");
        print_usage(argv[0]);
        exit(1);
    }
  }
  if (args->help)
  {
    return;
  }

  if ((optind+1) == argc)
  {
    args->name = argv[optind];
  }
  else if ((optind+1) > argc)
  {
    fprintf(stderr, "ERROR: Missing name of the domain to be translated\n");
    print_usage(argv[0]);
    // printf("optind = %d\n", optind);
    // printf("argc = %d\n", argc);
    exit(1);
  }
  else
  {
    fprintf(stderr, "ERROR: Unknown arguments passed\n");
    print_usage(argv[0]);
    exit(1);
  }
  if (!args->server) {
    fprintf(stderr, "ERROR: Missing server parameter\n");
    print_usage(argv[0]);
    exit(1);
  }
}
