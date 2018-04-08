/******** iterative.h - IPK Project 2 **********
* Author: Jan Koci
* Date: 24.3.2018
* Brief: functions for iterative query resolution
************************************************/
#ifndef ITERATIVE_H
#define ITERATIVE_H
#include <stdint.h>

extern uint16_t A;
extern uint16_t NS;
extern uint16_t CNAME;
extern uint16_t PTR;
extern uint16_t AAAA;
extern uint16_t IN;

/**
* @brief search for specific record type in dns answer
* @param message    start of dns message
* @param ans_start  start of dns answer
* @param type       type to look for
* @param dest       store answer data here
* @return           1 when record found
*                   0 when record not found, but NS or CNAME record was found
*                   -1 when record not found and neither NS or CNAME
*/
int find_answer(const uint8_t* message, const uint8_t* ans_start, uint16_t type, uint8_t* dest);

/**
* @brief send dns queries repeatedly until specific type of answer record is found
*        or an error occurs
* @param client_socket    socket for udp communication
* @param message          buffer to storing dns message data
* @param server           server to send the query to
* @param stype            string representation of the answer type we look for
* @param name             name that we query about in dns question
* @param dest             destination to store answer data
* @return     0 when success
*            -1 when an error occurs
*/
int get_answer(int client_socket, uint8_t* message, const char* server, \
               char* stype, char* name, uint8_t* dest);

/**
* @brief convert string representation of dns record type into uint16_t
* @param type  string representation of the type
* @return      uint16_t representation of the type
*/
uint16_t to_uint_type(char* type);


#endif
