#ifndef DNS_CONVERT_H
#define DNS_CONVERT_H

#include <stdint.h>

void readIP(const uint8_t* src, uint8_t* dest, int ipv);
int parse_iter_domain(const char* domain, int point, int len);
void print_ipv6(const uint8_t* data);
int get_IPv(uint8_t* addr);
int count_groups(const uint8_t* addr, char delimiter);
void convert_ipv6(const uint8_t* src, uint8_t* dest);
void convert_ipv4(const uint8_t* src, uint8_t* dest);
void get_reverse(const uint8_t* src, uint8_t* dest, int ipv);

#endif //DNS_CONVERT_H
