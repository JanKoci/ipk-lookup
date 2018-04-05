#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
// #include <stdlib.h>
#include "dns_convert.h"

int get_IPv(uint8_t* addr)
{
  unsigned char temp_buff[46] = {0};
  if (inet_pton(AF_INET, (const char*)addr, temp_buff) == 1)
  {
    return 4;
  }
  else if (inet_pton(AF_INET6, (const char*)addr, temp_buff) == 1)
  {
    return 6;
  }
  else
  {
    return -1;
  }
}


int count_groups(const uint8_t* addr)
{
  int len = strlen((const char*)addr);
  int counter = 0;
  int group_counter = 0;
  for (int i = 0; i < len; i++)
  {
    if (addr[i] == ':')
    {
      if (counter > 0)
      {
        group_counter++;
        counter = 0;
      }
    }
    else
    {
      counter++;
    }
  }

  if (counter > 0)
  {
    group_counter++;
  }
  return group_counter;
}

void convert_ipv6(const uint8_t* src, uint8_t* dest)
{
  int len = strlen((const char*)src);
  int counter = 0;
  uint8_t c;
  uint8_t c_prev = 0;
  int groups = count_groups(src);

  for (int i = 0; i < len; i++) {
    c = src[len-i-1];
    if (c == ':')
    {
      if (counter == 0)
      {
        // substitute
        if (c_prev == ':')
        {
          for (int j = 0; j < (8-groups)*4; j++)
          {
            *dest++ = '0';
            *dest++ = '.';
          }
        }
      }
      else if (counter != 4)
      {
        while (counter != 4) {
          *dest++ = '0';
          *dest++ = '.';
          counter++;
        }
      }
      counter = 0;
    }
    else
    {
      *dest++ = c;
      *dest++ = '.';
      counter++;
    }
    c_prev = c;
  }
  dest--;
  *dest++ = '\0';
}

void convert_ipv4(const uint8_t* src, uint8_t* dest)
{
  int len = strlen((const char*)src);
  int counter = 0;
  src += len-1;

  for (int i = 0; i < len; i++)
  {
    if (*src == '.')
    {
      for (int j = 1; j <= counter; j++)
      {
        *dest++ = src[j];
      }
      *dest++ = *src;
      counter = 0;
    }
    else
    {
      counter++;
    }
    src--;
  }
  for (int j = 1; j <= counter; j++)
  {
    *dest++ = src[j];
  }
}


void get_reverse(const uint8_t* src, uint8_t* dest, int ipv)
{
  size_t len = strlen((char*)src);
  if (ipv == 4)
  {
    convert_ipv4(src, dest);
    strcat((char*)dest, ".in-addr.arpa");
  }
  else if (ipv == 6)
  {
    convert_ipv6(src, dest);
    strcat((char*)dest, ".ip6.arpa");
  }
}


/************************* USAGE *************************/
// int main(int argc, char const *argv[]) {
//   int ipv;
//   uint8_t ip_name[128] = {0};
//   if (argc != 2)
//   {
//     return 1;
//   }
//   if ((ipv = get_IPv((uint8_t*)argv[1])) == -1)
//   {
//     printf("ERROR: invalid IP %s\n", argv[1]);
//     return 1;
//   }
//   get_reverse((const uint8_t*)argv[1], ip_name, ipv);
//   printf("%s\n", ip_name);
//   return 0;
// }
