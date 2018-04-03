/******** ipk-lookup.h - IPK Project 2 **********
* Author: Jan Koci
* Date: 24.3.2018
* Brief: basic tool for communication with DNS servers
        using socket programming and UDP protocol
************************************************/

#ifndef IPK_LOOKUP_H
#define IPK_LOOKUP_H
#include <stdint.h>

#define MESSAGE_LEN 512 // lenght of a DNS message
#define PORT 53 // DNS port
#define ANSWER_SIZE 10

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

// DNS message answer (Resource Record)
struct dns_answer {
  // name: name of the queried address
  uint16_t type;
  uint16_t cls;
  uint32_t ttl;
  uint16_t data_len;
  // record_data: [Address: 172.217.23.226] (query type was A)
};

const uint8_t* getDnsName(uint8_t* converted_name, const uint8_t* dns_name, const uint8_t* base);
uint8_t* toDnsNameFormat(uint8_t* dnsName, const uint8_t* host);

#endif // IPK_LOOKUP_H
