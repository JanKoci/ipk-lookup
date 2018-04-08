/******** ipk-lookup.h - IPK Project 2 **********
* Author: Jan Koci
* Date: 24.3.2018
* Brief: basic tool for communication with DNS servers
        using socket programming and UDP protocol
************************************************/

#ifndef IPK_LOOKUP_H
#define IPK_LOOKUP_H
#include <stdint.h>
#include <stdbool.h>

#define MESSAGE_LEN 512 // lenght of a DNS message
#define PORT 53 // DNS port
#define ANSWER_SIZE 10
#define IPV4_LEN 4
#define IPV6_LEN 16

// DNS message header
// note: both DNS query and response have the same
//       format of message
struct dns_header {
  uint16_t id;
  uint16_t flags;
  uint16_t q_count; // number of questions
  uint16_t ans_count; // number of answer resource records (RRs)
  uint16_t ns_count; // number of authority RRs
  uint16_t ar_count; // number of additional RRs
};

// DNS message question
struct dns_question {
  // name: name of the address to be translated
  uint16_t type; // A, AAAA, CNAME, PTR, NS
  uint16_t cls; // 1 = Internet
};

#pragma pack(push,1)
// DNS message answer (Resource Record)
struct dns_answer {
  // name: name of the queried address
  uint16_t type;
  uint16_t cls;
  uint32_t ttl;
  uint16_t data_len;
  // record_data: [Address: 172.217.23.226] (query type was A)
};
#pragma pack(pop)

void clean_array(uint8_t* array, int size);
uint16_t to_uint_type(char* type);
int find_answer(const uint8_t* message, const uint8_t* ans_start, uint16_t type, uint8_t* dest);
int get_answer(int client_socket, uint8_t* message, const char* server, \
               char* stype, char* name, uint8_t* dest);
int send_query_and_receive_answer(const char*, int, uint8_t* const, unsigned int);
int create_query(uint8_t* const message, const char* type, const char* name);
void create_header(struct dns_header* header);
void create_question(struct dns_question* question, const char* type);
void process_answers(const uint8_t* message, const uint8_t* dns_name, uint16_t ans_count);
const uint8_t* getDnsName(uint8_t* converted_name, const uint8_t* dns_name, const uint8_t* base);
uint8_t* toDnsNameFormat(uint8_t* dnsName, const uint8_t* host);
int get_IPv(uint8_t* addr);

#endif // IPK_LOOKUP_H
