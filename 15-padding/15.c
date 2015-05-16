#include <string.h>
#include "../lib/libkrypto.h"

int main(void)
{
  char plaintext[] = "ICE ICE BABY\x04\x04\x04\x04";
  int i;

  i = padding_detect(plaintext, strlen(plaintext), 20);
  i = padding_detect((char *)"ICE ICE BABY\x05\x05\x05\x05", strlen("ICE ICE BABY\x05\x05\x05\x05"), 20);
  i = padding_detect((char *)"ICE ICE BABY\x01\x02\x03\x04", strlen("ICE ICE BABY\x01\x02\x03\x04"), 20);
  i = padding_detect((char *)"ICE ICE BABY", strlen("ICE ICE BABY"), 20);

  printf("\nstring = %s\n", "ICE ICE BABY1234");
  i = padding_detect((char *)"ICE ICE BABY1234", strlen("ICE ICE BABY1234"), 20);

  return 0;
}
