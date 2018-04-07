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
#include "dns_convert.h"

// types of DNS query
uint16_t A = 1;
uint16_t NS = 2;
uint16_t CNAME = 5;
uint16_t PTR = 12;
uint16_t AAAA = 28;
uint16_t IN = 1;
// to set recursion bit in DNS message flags
uint16_t RECURSIVE = 256; // 0000 0001 0000 0000
uint16_t port = PORT; // PORT 53


int main(int argc, char const *argv[]) {
  int client_socket;
  uint8_t message[MESSAGE_LEN] = {0};
  int query_len = 0;
  struct timeval timeout;

  Arguments args;

  init_args(&args);
  parse(argc, argv, &args);
  if (args.help)
  {
    print_help(argv[0]);
    exit(0);
  }
  timeout.tv_sec = args.timeout;
  timeout.tv_usec = 0;

  /************************* Create socket *************************/
  if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("ERROR: Cannot create socket");
    exit(1);
  }
  // set receive timeout
  if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, \
                  sizeof(timeout)) < 0)
  {
    perror("ERROR: setsockopt failed");
    exit(1);
  }

  /************************* Create the query *************************/
  query_len = create_query(message, args.type, args.name);
  if (query_len == -1)
  {
    close(client_socket);
    exit(1);
  }

  // debug
  // hex_dump((unsigned char*)message, message_len_actual);

  /************************* Send the query *************************/
  if (send_query_and_receive_answer(args.server, client_socket, (uint8_t* const)message, query_len) == -1)
  {
    close(client_socket);
    exit(1);
  }

  /************************* Process the answer *************************/
  struct dns_header* header = (struct dns_header*)message;
  uint16_t ans_count = ntohs(header->ans_count);
  // uint16_t ns_count = ntohs(header->ns_count);
  // uint16_t ar_count = ntohs(header->ar_count);

  // debug
  // printf("ans_count = %hu\n", ans_count);
  // printf("ns_count = %hu\n", ns_count);
  // printf("ar_count = %hu\n", ar_count);

  process_answers((const uint8_t*)message, (const uint8_t*)&message[query_len], ans_count);

  /************************* Close socket *************************/
  close(client_socket);
  return 0;
}


int send_query_and_receive_answer(const char* server, int client_socket, uint8_t* const message, unsigned int query_len)
{
  struct sockaddr_in server_addr;

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(server);

  socklen_t server_len = (socklen_t)sizeof(server_addr);
  ssize_t bytestx;
  if ((bytestx = sendto(client_socket, message, query_len, 0, \
      (struct sockaddr*)&server_addr, server_len)) < 0)
  {
    perror("ERROR: sendto");
    return -1;
  }
  /************************* Receive answer *************************/
  if ((bytestx = recvfrom(client_socket, message, MESSAGE_LEN, 0, \
        (struct sockaddr*)&server_addr, &server_len)) < 0)
  {
    perror("ERROR: recv");
    return -1;
  }
  return 0;
}

int create_query(uint8_t* const message, const char* type, const char* name)
{
  unsigned int message_len_actual = 0;
  /************************* Create DNS header *************************/
  struct dns_header* header;
  header = (struct dns_header*)message;
  create_header(header);
  message_len_actual += sizeof(struct dns_header);

  uint8_t* q_name = &message[message_len_actual];
  if (strcmp(type, "PTR") == 0)
  {
    int ipv;
    char new_name[128] = {0};
    if ((ipv = get_IPv((uint8_t*)name)) == -1)
    {
      fprintf(stderr, "ERROR: invalid IP address %s\n", name);
      return -1;
    }
    get_reverse((const uint8_t*)name, (uint8_t*)new_name, ipv);
    name = new_name;
  }
  uint8_t* result = toDnsNameFormat(q_name, (const uint8_t*)name);
  message_len_actual += (strlen((char*)result) + 1);
  // debug
  // hex_dump((unsigned char*)result, strlen((char*)result));
  // putchar('\n');
  // hex_dump((unsigned char*)message, sizeof(struct dns_header)+strlen((char*)result));

/************************* Create question structure *************************/
  struct dns_question* question;
  question = (struct dns_question*)&message[message_len_actual];
  create_question(question, type);
  message_len_actual += sizeof(struct dns_question);
  return message_len_actual;
}


void create_header(struct dns_header* header)
{
  header->id = htons(4); // what ID ???
  header->flags = 0;
  header->flags |= htons(RECURSIVE); // ALWAYS ?????
  header->q_count = htons(1);
  header->ans_count = 0;
  header->ns_count = 0;
  header->ar_count = 0;
}

void create_question(struct dns_question* question, const char* type)
{
  if (strcmp(type, "A") == 0)
  {
    question->type = htons(A);
  }
  else if (strcmp(type, "AAAA") == 0)
  {
    question->type = htons(AAAA);
  }
  else if (strcmp(type, "NS") == 0)
  {
    question->type = htons(NS);
  }
  else if (strcmp(type, "PTR") == 0)
  {
    question->type = htons(PTR);
  }
  else if (strcmp(type, "CNAME") == 0)
  {
    question->type = htons(CNAME);
  }
  question->cls = htons(IN);
}

void process_answers(const uint8_t* message, const uint8_t* ans_start, uint16_t ans_count)
{
  uint8_t converted_name[128] = {0};
  struct dns_answer* net_answer;
  struct dns_answer answer;
  const uint8_t* ret;

  // process each answer
  for (uint16_t i = 0; i < ans_count; i++)
  {
    //Process answer
    ret = getDnsName(converted_name, (const uint8_t*)ans_start, (const uint8_t*)message);
    net_answer = (struct dns_answer*)ret;
    answer.type = ntohs(net_answer->type);
    answer.cls = ntohs(net_answer->cls);
    answer.ttl = ntohl(net_answer->ttl);
    answer.data_len = ntohs(net_answer->data_len);
    printf("%s ", converted_name);
    // debug
    // printf("%c\n", *ret);
    print_ans(&answer);
    const uint8_t* rdata = ret + ANSWER_SIZE;
    ans_start = rdata + answer.data_len;
    if (answer.type == A)
    {
      for (uint16_t j = 0; j < answer.data_len; j++)
      {
        printf("%u", *rdata++);
        if (j+1 != answer.data_len)
        {
          putchar('.');
        }
      }
    }
    else if (answer.type == CNAME || answer.type == NS || answer.type == PTR)
    {
      ret = getDnsName(converted_name, (const uint8_t*)rdata, (const uint8_t*)message);
      printf("%s ", converted_name);
      if (ret != ans_start)
      {
        ans_start = ret;
      }
    }
    else if (answer.type == AAAA)
    {
      print_ipv6(rdata);
    }
    putchar('\n');
    memset(converted_name, 0, 128);
    memset(&answer, 0, sizeof(answer));
  }
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

  // if first Byte is 0 it is the ROOT server .
  if (*dns_name == 0)
  {
    *converted_name++ = '.';
    *converted_name++ = '\0';
  }

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
      if (ret_ptr2 == NULL)
      {
        ret_ptr2 = dns_name + 2;
      }
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
  if (*host != '.')
  {
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
  }
  *dnsName = '\0';
  dnsName++;
  return start;
}
