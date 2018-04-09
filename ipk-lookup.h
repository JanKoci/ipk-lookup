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


/**
* @brief clean array data (fill array with zero bytes)
* @param array  the array to clean
* @param size   size of the array
*/
void clean_array(uint8_t* array, int size);

/**
* @brief send dns message (query) to server
* @param server         server to send the query to
* @param client_socket  socket for udp communication
* @param message        pointer to the dns message
* @param query_len      length of the dns message
* @return   0 when success
*          -1 when an error occurs
*/
int send_query_and_receive_answer(const char*, int, uint8_t* const, unsigned int);

/**
* @brief create dns query message
* @param message  buffer to stores the message
* @param type     type of dns query
* @param name     the domain name / IP address to query about
* @return    lenght of the message
*/
int create_query(uint8_t* const message, const char* type, const char* name);

/**
* @brief create dns message header
* @param header   pointer to the dns header structure
*/
void create_header(struct dns_header* header);

/**
* @brief fill the dns_question structure
* @param question   the dns_question structure
* @param type       dns question type
*/
void create_question(struct dns_question* question, const char* type);

/**
* @brief print dns answers
* @param message    the dns message containing answers
* @param ans_start  start of the first answer
* @param ans_count  number of answers to print
*/
bool process_answers(const uint8_t* message, const uint8_t* ans_start, uint16_t ans_count, uint16_t type);

/**
* @brief converts encoded domain name from dns format to string representation
* Example: 03www03fit02cz => www.fit.cz
* @param converted_name   store here the converted domain name
* @param dns_name         stores the original data
* @param base             pointer to the start of the dns message
* @return pointer behind the last answer
*/
const uint8_t* getDnsName(uint8_t* converted_name, const uint8_t* dns_name, const uint8_t* base);

/**
* @brief encodes domain name string representation into dns required format
* Example: www.fit.cz => 03www03fit02cz
* @param dnsName  store here the converted data
* @param host     stores domain's string representation
* @return pointer to the beginning of converted data in dnsName
*/
uint8_t* toDnsNameFormat(uint8_t* dnsName, const uint8_t* host);

#endif // IPK_LOOKUP_H
