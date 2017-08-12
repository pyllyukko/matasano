#include <string.h>
#include "../lib/libkrypto.h"
#include "../lib/hacking.h"

/*
 * math:
 *   to skip one block, we need 11 bytes, to get 2 full blocks we need 32
 *
 *
 *
 */

int main(void)
{
  char
    plaintext[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
    ciphertext[128];
  int length;

  length = encryption_oracle(plaintext, ciphertext, strlen(plaintext));
  printf("\nciphertext length=%d\n", length);
  printf("adversary: encryption mode is ");
  if(memcmp(ciphertext+16, ciphertext+32, 16)==0)
  {
    printf("\033[1mECB\033[0m\n");
  } else {
    printf("\033[1mCBC\033[0m\n");
  }
  printf("\nciphertext:\n");
  dump(ciphertext, length);
}
