/******** ipk-lookup.h - IPK Project 2 **********
* Author: Jan Koci
* Date: 24.3.2018
* Brief: basic tool for communication with DNS servers
        using socket programming and UDP protocol
************************************************/
#ifndef IPK_LOOKUP_H
#define IPK_LOOKUP_H

#define MESSAGE_LEN 512 // lenght of a DNS message
#define PORT 53 // DNS port

// types of DNS query
#define A 1
#define NS 2
#define CNAME 5
#define PTR 12
#define AAAA 28

// to set recursion bit in DNS message flags
unsigned short RECURSIVE = 256; // 0000 0001 0000 0000

// DNS message header
// note: both DNS query and response have the same
//       format of message
struct dns_header {
  unsigned short id;
  unsigned short flags;
  unsigned short q_count; // number of questions
  unsigned short ans_count; // number of answer resource records (RRs)
  unsigned short ns_count; // number of authority RRs
  unsigned short ar_count; // number of additional RRs
};

// DNS message question
struct dns_question {
  // name: name of the address to be translated
  unsigned short type; // A, AAAA, CNAME, PTR, NS
  unsigned short cls; // 1 = Internet
};

// DNS message answer (Resource Record)
struct dns_answer {
  // name: name of the queried address
  unsigned short type;
  unsigned short cls;
  unsigned int ttl;
  unsigned short data_len;
  // record_data: [Address: 172.217.23.226] (query type was A)
};


unsigned char* toDnsNameFormat(unsigned char* dnsName, const unsigned char* host);

#endif // IPK_LOOKUP_H
