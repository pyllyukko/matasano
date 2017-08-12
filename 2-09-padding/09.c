#include <stdio.h>
#include <string.h>
#include "../lib/libkrypto.h"
#include "../lib/hacking.h"

/*
 * matasano 9. Implement PKCS#7 padding
 *
 * TODO:
 *   15. PKCS#7 padding validation
 *
 * NOTES:
 *   - https://en.wikipedia.org/wiki/Padding_%28cryptography%29#PKCS7
 */
int main(void)
{
  int i;
  char plaintext[20] = "YELLOW SUBMARINE";
  char plaintext2[40] = "YELLOW SUBMARINEabcbd";

  printf("main (before padding):\n");
  dump(plaintext, 20);

  //*plaintext = "YELLOW SUBMARINE";
  pad(plaintext, strlen(plaintext), 20);

  printf("main (after padding):\n");
  dump(plaintext, 20);
  printf("\n");

  printf("main (before padding):\n");
  dump(plaintext2, 40);

  //*plaintext = "YELLOW SUBMARINE";
  pad(plaintext2, strlen(plaintext2), 20);

  printf("main (after padding):\n");
  dump(plaintext2, 40);
  printf("\n");

  padding_detect(plaintext, strlen(plaintext), 20);
  printf("strlen(plaintext2)=%d\n", strlen(plaintext2));
  padding_detect(plaintext2, 40, 20);
}
