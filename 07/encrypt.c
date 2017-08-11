#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "../lib/libkrypto.h"
#include "../lib/hacking.h"

static const unsigned char key[] = "YELLOW SUBMARINE";
static const unsigned char iv[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

int main(void)
{
  unsigned char
    plaintext[] = "I'm back and I'm ringin' the bell",
    ciphertext[128],
    randomized_key[40];
  int i, keylen;

  aes_encrypt_ecb(plaintext, ciphertext, strlen(plaintext), (char *)key, strlen(key));
  dump(ciphertext, strlen(key));

  base64_encode("Hello World \n", stdout);

  srand(time(0));

  keylen = rand() % 39 + 2;
  printf("keylen=%d\n", keylen);
  randomize_key(randomized_key, keylen);
  dump(randomized_key, keylen);

  return 0;
}
