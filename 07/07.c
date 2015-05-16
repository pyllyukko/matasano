/*
 * code for the matasano crypto challenge #7
 *
 * it's either this... or then we could just run:
 * openssl enc -d -aes-128-ecb -nosalt -K `echo -n 'YELLOW SUBMARINE'|hexdump -v -e '/1 "%02X"'` -d -in ciphertext.bin
 *
 */
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
//#include <openssl/aes.h>
#include "../lib/libkrypto.h"

static const unsigned char key[] = "YELLOW SUBMARINE";

int main(void)
{

  unsigned char ciphertext[4096], plaintext[4096];
  //AES_KEY enc_key, dec_key;
  FILE *fp;
  int
    i, j, padding,
    ciphertext_length = 0,
    keysize = strlen(key);

  printf("keysize=%d (%d bits)\n\n", keysize, keysize*8);

  fp = fopen("./07.txt", "r");
  if(fp == NULL)
  {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  ciphertext_length = base64_decode(fp, ciphertext, sizeof(ciphertext));
  /*
  if((ciphertext_length = base64_decode(fp, ciphertext, sizeof(ciphertext)))==-1)
  {
    printf("error: bailing out!\n");
    fclose(fp);
    exit(EXIT_FAILURE);
  } else {
    printf("  read total %d bytes (%d blocks)\n\n", ciphertext_length, ciphertext_length/keysize);
  }
  */
  fclose(fp);
  if(ciphertext_length==0)
  {
    printf("error: bailing out!\n");
    exit(EXIT_FAILURE);
  }

  printf("ciphertext:\n");
  // print a sample of the  ciphertext:
  /*
  for(i=0; i<4; i++)
  {
    printf("  %d = %.2x\n", i, ciphertext[i]);
  }
  for(i=ciphertext_length-4; i<ciphertext_length; i++)
  {
    printf("  %d = %.2x\n", i, ciphertext[i]);
  }
  */
  printf("  ");
  dump(ciphertext, 16);
  printf("  ...\n  ");
  dump(ciphertext+ciphertext_length-16, 16);

  /*
  AES_set_decrypt_key(key, keysize*8, &dec_key);
  printf("\ndecrypting");
  for(i=0; i<ciphertext_length; i+=keysize)
  {
    AES_decrypt(ciphertext+i, plaintext+i, &dec_key);
    printf(".");
  }
  printf("\n\n");
  */
  ciphertext_length = aes_decrypt(ciphertext, plaintext, ciphertext_length, (char *)key, keysize);
  //printf("DEBUG: ciphertext_length=%d\n", ciphertext_length);

  /*
  padding = padding_detect(plaintext, ciphertext_length, keysize);
  printf("\npadding=%d\n\n", padding);
  ciphertext_length-=padding;
  */

  printf("\ndecrypted:\n");
  for(i=0;i<ciphertext_length;i++)
  {
    printf("%c", plaintext[i]);
  }

  return 0;
}
