/******** dns_convert.c - IPK Project 2 **********
* Author: Jan Koci
* Date: 24.3.2018
* Brief: functions to convert string representation of a IP address,
*        entered from the user to correct dsn format
* Example: 123.54.63.4 => 4.63.54.123.in-addr.arpa
************************************************/
#ifndef DNS_CONVERT_H
#define DNS_CONVERT_H
#include <stdint.h>

/**
* @brief read IP4/IPv6 address represented as 4/16 bytes into a string representation
* @param src    source of the IP address
* @param dest   store here the converted result
* @param ipv    either 4 or 6 (IP version)
*/
void readIP(const uint8_t* src, uint8_t* dest, int ipv);

/**
* @brief parse domain name gradually from the end using dots '.'
* Example: www.fit.vutbr.cz => vutbr.cz
* @param domain the domain name to be parsed
* @param point  says where the last split in the domain was
* @param len    length of the domain name
* @return       index (new point) in the domain
*/
int parse_iter_domain(const char* domain, int point, int len);

/**
* @brief print readable representation of an IPv6 address
* @param data  the address to be print
*/
void print_ipv6(const uint8_t* data);

/**
* @bried get version the IP address
* @param addr   the IP address we want to get type of
* @return       4 when addr is IPv4
                6 when addr is IPv6
               -1 when addr is unknown IPv
*/
int get_IPv(uint8_t* addr);

/**
* @brief count groups of characters separated by delimiter in string
* @param addr       string to count groups in
* @param delimiter  delimiter of groups
*/
int count_groups(const uint8_t* addr, char delimiter);

/**
* @brief convert string representation of an IPv6 address into a format in which
*        it can be send in a dns message
* Example: 2001:67c:1220:8b0::93e5:b013 => 3.1.0.b.5.e.3.9.0.0.0.0.0.0.0.0.0.b.8.0.0.2.2.1.c.7.6.0.1.0.0.2
* @param src  string representation of the address
* @param dest store here the converted address
*/
void convert_ipv6(const uint8_t* src, uint8_t* dest);

/**
* @brief convert string representation of an IPv4 address into a format in which
*        it can be send in a dns message
* Example: 128.43.13.5 => 5.13.43.128
* @param src  string representation of the address
* @param dest store here the converted address
*/
void convert_ipv4(const uint8_t* src, uint8_t* dest);

/**
* @brief convert string representation of an IP address into a format in which
*        it can be send in a dns message
* Example: 128.43.13.5 => 5.13.43.128.in-addr.arpa
* @param src  string representation of the address
* @param dest store here the converted address
* @param ipv  IP version of the address in src
*/
void get_reverse(const uint8_t* src, uint8_t* dest, int ipv);

#endif //DNS_CONVERT_H
