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
#include <arpa/inet.h>
#include <string.h>

#include "ipk-lookup.h"
#include "hex_dump.h"
#include "args_parser.h"

// types of DNS query
uint16_t A = 1;
uint16_t NS = 2;
uint16_t CNAME = 5;
uint16_t PTR = 12;
uint16_t AAAA = 28;
uint16_t IN = 1;
// to set recursion bit in DNS message flags
uint16_t RECURSIVE = 256; // 0000 0001 0000 0000


int main(int argc, char const *argv[]) {
  int client_socket;
  struct sockaddr_in server_addr;
  uint8_t message[MESSAGE_LEN] = {0}; // unsigned ?
  unsigned int message_len_actual = 0;

  uint16_t port = PORT; // PORT 53
  Arguments args;
  // (void) port;
  // (void) server_addr;

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
  header->id = htons(4); // what ID ???
  header->flags = 0;
  if (!args.iterative) {
    header->flags |= htons(RECURSIVE);
  }
  header->q_count = htons(1); //htons(1) ???
  header->ans_count = 0;
  header->ns_count = 0;
  header->ar_count = 0;
  message_len_actual += sizeof(struct dns_header);

  uint8_t* q_name = &message[message_len_actual];
  uint8_t* result = toDnsNameFormat(q_name, (const uint8_t*)args.name);
  message_len_actual += (strlen((char*)result) + 1);
  // hex_dump((char*)result, strlen((char*)result));
  // putchar('\n');
  // hex_dump((char*)message, sizeof(struct dns_header)+strlen((char*)result));

  // function toDnsNameFormat moves q_name pointer behind the domain name
  struct dns_question* question;
  question = (struct dns_question*)&message[message_len_actual];
  if (strcmp(args.type, "A") == 0)
  {
    question->type = htons(A);
  }
  else if (strcmp(args.type, "AAAA") == 0)
  {
    question->type = htons(AAAA);
  }
  else if (strcmp(args.type, "NS") == 0)
  {
    question->type = htons(NS);
  }
  else if (strcmp(args.type, "PTR") == 0)
  {
    question->type = htons(PTR);
  }
  else if (strcmp(args.type, "CNAME") == 0)
  {
    question->type = htons(CNAME);
  }
  question->cls = htons(IN);
  message_len_actual += sizeof(struct dns_question);
  // hex_dump((char*)message, message_len_actual);

  /************** Send the query **************/
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(args.server);

  socklen_t server_len = (socklen_t)sizeof(server_addr);
  ssize_t bytestx;
  if ((bytestx = sendto(client_socket, &message, message_len_actual, 0, \
      (struct sockaddr*)&server_addr, server_len)) < 0)
  {
    perror("ERROR: sendto");
    exit(1);
  }
  /************** Receive answer **************/
  if ((bytestx = recvfrom(client_socket, &message, MESSAGE_LEN, 0, \
        (struct sockaddr*)&server_addr, &server_len)) < 0)
  {
    perror("ERROR: recv");
    exit(1);
  }

  /************** Process the answers **************/
  header = (struct dns_header*)&message;
  uint16_t ans_count = ntohs(header->ans_count);
  uint16_t ns_count = ntohs(header->ns_count);
  uint16_t ar_count = ntohs(header->ar_count);

  printf("ans_count = %hu\n", ans_count);
  printf("ns_count = %hu\n", ns_count);
  printf("ar_count = %hu\n", ar_count);

  uint8_t* dns_name = &message[message_len_actual];
  uint8_t converted_name[128] = {0};
  struct dns_answer* net_answer;
  struct dns_answer answer;
  const uint8_t* ret;

  // process each answer
  for (uint16_t i = 0; i < ans_count; i++)
  {
    //Process answer
    ret = getDnsName(converted_name, (const uint8_t*)dns_name, (const uint8_t*)&message);
    net_answer = (struct dns_answer*)ret;
    answer.type = ntohs(net_answer->type);
    answer.cls = ntohs(net_answer->cls);
    answer.ttl = ntohl(net_answer->ttl);
    answer.data_len = ntohs(net_answer->data_len);
    printf("%s ", converted_name);
    // printf("%c\n", *ret);
    print_ans(&answer);
    const uint8_t* ptr = ret + ANSWER_SIZE;
    for (uint16_t i = 0; i < answer.data_len; i++)
    {
      printf("%u.", *ptr++);
    }
    putchar('\n');
    memset(converted_name, 0, 128);
    memset(&answer, 0, sizeof(answer));
  }

  close(client_socket);
  printf("Socket closed!\n");
  return 0;
}


const uint8_t* getDnsName(uint8_t* converted_name, const uint8_t* dns_name, const uint8_t* base)
{
  /* read first byte
    - if it the first two bits are 11 => pointer
    - else (bits are 00) => labels */
  uint8_t mask = 192; // 0b11000000
  uint8_t byte;
  const uint8_t* ret_ptr1 = dns_name;
  const uint8_t* ret_ptr2 = NULL;

  while ((byte = *dns_name) != 0)
  {
    if ((byte & mask) == 0)
    {
      // label => byte is num of letters
      dns_name++;
      for (uint8_t i = 0; i < byte; i++)
      {
        *converted_name++ = *dns_name++;
      }
      *converted_name++ = '.';
      ret_ptr1 = dns_name;
    }
    else if ((byte & mask) == 192)
    {
      // pointer
      uint16_t* ptr = (uint16_t*)dns_name;
      uint16_t offset = ntohs(*ptr);
      offset &= 16383;  // 0b0011111111111111
      ret_ptr2 = dns_name + 2;
      dns_name = base + offset; // uint8_t* + uint16_t* ???????
    }
    else
    {
      // undefined state
      exit(1);
    }
  }
  converted_name--;
  *converted_name = '\0';
  if (ret_ptr2 != NULL)
  {
    return ret_ptr2;
  }
  return ret_ptr1+1;
}


/*
Function to convert hostname to dns host name format
@param dnsName    pointer to memory to store the converted value
@param host       the hostname to be converted
@return           pointer to the beginning of the converted value
*/
uint8_t* toDnsNameFormat(uint8_t* dnsName, const uint8_t* host)
{
  unsigned int point = 0;
  uint8_t* start = dnsName;
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
