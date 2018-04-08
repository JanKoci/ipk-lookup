/******** iterative.c - IPK Project 2 **********
* Author: Jan Koci
* Date: 24.3.2018
* Brief: functions for iterative query resolution
************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipk-lookup.h"
#include "dns_convert.h"
#include "iterative.h"


int find_answer(const uint8_t* message, const uint8_t* ans_start, uint16_t type, uint8_t* dest)
{
  struct dns_header* header = (struct dns_header*)message;
  uint16_t ans_count = ntohs(header->ans_count);
  uint16_t ns_count = ntohs(header->ns_count);
  uint16_t ar_count = ntohs(header->ar_count);
  const uint8_t* ret;
  struct dns_answer* net_answer;
  struct dns_answer answer;
  const uint8_t* rdata;
  const uint8_t* optional_data;
  const uint8_t* optional_ans_start;
  uint8_t converted_name[128] = {0};
  int found = -1;
  int optional_found = 0;

  for (uint16_t i = 0; i < ans_count+ns_count+ar_count; i++)
  {
    ret = getDnsName(converted_name, (const uint8_t*)ans_start, (const uint8_t*)message);
    net_answer = (struct dns_answer*)ret;
    answer.type = ntohs(net_answer->type);
    answer.cls = ntohs(net_answer->cls);
    answer.ttl = ntohl(net_answer->ttl);
    answer.data_len = ntohs(net_answer->data_len);
    rdata = ret + ANSWER_SIZE;
    if (answer.type == type)
    {
      found = 1;
      if (type == A)
      {
        readIP(rdata, dest, 4);
      }
      else if (type == AAAA)
      {
        readIP(rdata, dest, 6);
      }
      else
      {
        getDnsName(dest, rdata, message);
      }
      process_answers(message, ans_start, 1);
      break;
    }

    else if (answer.type == NS || answer.type == CNAME)
    {
      if (!optional_found)
      {
        optional_data = rdata;
        optional_ans_start = ans_start;
        optional_found = 1;
      }
    }
    ans_start = rdata + answer.data_len;

  }
  if (found == -1 && optional_found)
  {
    getDnsName(dest, optional_data, message);
    process_answers(message, optional_ans_start, 1);
    found = 0;
  }
  return found;
}



int get_answer(int client_socket, uint8_t* message, const char* server, \
               char* stype, char* name, uint8_t* dest)
{
  int found = 0;
  uint16_t type;
  int query_len;
  uint8_t answer_data[128] = {0};
  memcpy(answer_data, name, strlen(name));

  if ((type = to_uint_type(stype)) == 0)
  {
    return -1;
  }

  while (!found)
  {
    /************************* Create the query *************************/
    query_len = create_query(message, stype, (const char*)answer_data);
    if (query_len == -1)
    {
      return -1;
    }

    /************************* Send the query *************************/
    if (send_query_and_receive_answer(server, client_socket, (uint8_t* const)message, query_len) == -1)
    {
      return -1;
    }
    /************************* Process the answer *************************/
    clean_array(answer_data, 128);
    if ((found = find_answer((const uint8_t*)message, (const uint8_t*)&message[query_len], type, answer_data)) == -1)
    {
      fprintf(stderr, "ERROR: %hu record not found in the answer\n", type);
      return -1;
    }
  }
  clean_array(dest, 128);
  memcpy(dest, answer_data, 128);
  return 0;
}


uint16_t to_uint_type(char* type)
{
  if (strcmp(type, "A") == 0)
  {
    return A;
  }
  if (strcmp(type, "AAAA") == 0)
  {
    return AAAA;
  }
  if (strcmp(type, "NS") == 0)
  {
    return NS;
  }
  if (strcmp(type, "CNAME") == 0)
  {
    return CNAME;
  }
  if (strcmp(type, "PTR") == 0)
  {
    return PTR;
  }
  else
  {
    fprintf(stderr, "ERROR: unknown record type %s\n", type);
    return 0;
  }
}
