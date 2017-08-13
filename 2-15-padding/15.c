#include <string.h>
#include "../lib/libkrypto.h"

int main(void)
{
  char plaintext[] = "ICE ICE BABY\x04\x04\x04\x04"; // valid padding

  printf("PKCS#7 padding validation\n\nvalid case:\n");
  padding_detect(plaintext, strlen(plaintext), 20);
  printf("\ntrying with \"\\x05\\x05\\x05\\x05\"\n");
  padding_detect((char *)"ICE ICE BABY\x05\x05\x05\x05", strlen("ICE ICE BABY\x05\x05\x05\x05"), 20); // invalid padding
  printf("\ntrying with \"\\x01\\x02\\x03\\x04\"\n");
  padding_detect((char *)"ICE ICE BABY\x01\x02\x03\x04", strlen("ICE ICE BABY\x01\x02\x03\x04"), 20); // invalid padding
  printf("\ntrying with no padding\n");
  padding_detect((char *)"ICE ICE BABY", strlen("ICE ICE BABY"), 20); // no padding

  printf("\nstring = %s\n", "ICE ICE BABY1234");
  padding_detect((char *)"ICE ICE BABY1234", strlen("ICE ICE BABY1234"), 20); // invalid padding

  return 0;
}
