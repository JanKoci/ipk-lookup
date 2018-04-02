/******** ipk-lookup.c - IPK Project 2 **********
* Author: Jan Koci
* Date: 24.3.2018
* Brief: basic tool for communication with DNS servers
        using socket programming and UDP protocol
************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include "ipk-lookup.h"
#include "hex_dump.h"
#include "args_parser.h"
extern unsigned short RECURSIVE;
extern unsigned short A;
extern unsigned short NS;
extern unsigned short CNAME;
extern unsigned short PTR;
extern unsigned short AAAA;
extern unsigned short IN;


int main(int argc, char const *argv[]) {
  int client_socket;
  struct sockaddr_in server_addr;
  unsigned char message[MESSAGE_LEN] = {0};
  unsigned short port = PORT; // PORT 53
  Arguments args;
  (void) port;
  (void) server_addr;

  init_args(&args);
  parse(argc, argv, &args);
  if (args.help)
  {
    print_help(argv[0]);
    exit(0);
  }

  // create socket
  if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("ERROR: Cannot create socket");
    exit(1);
  }
  printf("Socket created!\n");

  struct dns_header* header;
  header = (struct dns_header*)&message;
  header->id = 4; // what ID ???
  header->flags = 0;
  if (!args.iterative) {
    header->flags |= RECURSIVE;
  }
  header->q_count = 1; //htons(1) ???
  header->ans_count = 0;
  header->ns_count = 0;
  header->ar_count = 0;

  unsigned char* q_name = &message[sizeof(struct dns_header)];
  unsigned char* result = toDnsNameFormat(q_name, (const unsigned char*)args.name);
  hex_dump((char*)result, strlen((char*)result));
  putchar('\n');
  // hex_dump((char*)message, sizeof(struct dns_header)+strlen((char*)result));

  // function toDnsNameFormat moves q_name pointer behind the domain name
  struct dns_question* question;
  question = (struct dns_question*)&message[sizeof(struct dns_header)+strlen((char*)result)+1];
  if (strcmp(args.type, "A") == 0)
  {
    question->type = 1;
  }
  else if (strcmp(args.type, "AAAA") == 0)
  {
    question->type = AAAA;
  }
  else if (strcmp(args.type, "NS") == 0)
  {
    question->type = NS;
  }
  else if (strcmp(args.type, "PTR") == 0)
  {
    question->type = PTR;
  }
  else if (strcmp(args.type, "CNAME") == 0)
  {
    question->type = CNAME;
  }
  question->cls = 1;
  hex_dump((char*)message, sizeof(struct dns_header)+strlen((char*)result)+sizeof(struct dns_question)+1);


  close(client_socket);
  printf("Socket closed!\n");
  return 0;
}

/*
Function to convert hostname to dns host name format
@param dnsName    pointer to memory to store the converted value
@param host       the hostname to be converted
@return           pointer to the beginning of the converted value
*/
unsigned char* toDnsNameFormat(unsigned char* dnsName, const unsigned char* host)
{
  unsigned int point = 0;
  unsigned char* start = dnsName;
  strcat((char*)host, ".");

  for (unsigned int i = 0; i < strlen((char*)host); i++)
  {
    if (host[i] == '.')
    {
      *dnsName = i - point;
      dnsName++;
      for (;point < i; point++)
      {
        *dnsName = host[point];
        dnsName++;
      }
      point++;
    }
  }
  *dnsName = '\0';
  dnsName++;
  return start;
}
