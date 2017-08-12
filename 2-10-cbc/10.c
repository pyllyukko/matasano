#include <stdlib.h>
#include <string.h>
#include "../lib/libkrypto.h"
#include "../lib/hacking.h"

static const unsigned char key[] = "YELLOW SUBMARINE";
static const unsigned char iv[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

int main(void)
{
  FILE *fp;
  unsigned char ciphertext[4096], plaintext[4096];
  int plaintext_length, ciphertext_length;

  fp = fopen("./10.txt", "r");
  if(fp == NULL)
  {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }
  plaintext_length = base64_decode(fp, plaintext, sizeof(plaintext));
  fclose(fp);
  if(plaintext_length==0)
  {
    printf("error: bailing out!\n");
    exit(EXIT_FAILURE);
  }
  printf("read %d bytes\n", plaintext_length);
  dump(plaintext, 16);

  ciphertext_length = aes_encrypt_cbc(plaintext, ciphertext, plaintext_length, (char *)key, (char *)iv, strlen(key));

  dump(ciphertext, ciphertext_length);
}
