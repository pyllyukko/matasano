// https://www.cryptopals.com/sets/2/challenges/16

#include <stdio.h>
#include <string.h>
#include "../lib/libkrypto.h"
#include "../lib/hacking.h"

#define ANSI_COLOR_RED     "\x1b[1;31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_GREEN   "\x1b[1;32m"
#define BUFFER_SIZE 1024

char
  ciphertext[BUFFER_SIZE+16],
  key[16],
  iv[16];

int first_function(char *userdata, char *ciphertext)
{
  char userdata2[BUFFER_SIZE];
  int i, length, userdatalength;
  userdatalength = strlen(userdata);

  if(userdatalength>sizeof(userdata2)) {
    printf("[-] too much user data\n");
    return 1;
  }

  // input validation :)
  for(i=0; i<userdatalength; i++)
  {
    switch(*(userdata+i))
    {
      case ';':
	printf("[" ANSI_COLOR_RED "-" ANSI_COLOR_RESET "] first_function(): illegal character '%c'!\n", *(userdata+i));
	*(userdata+i) = 'X';
	break;
      case '=':
	printf("[" ANSI_COLOR_RED "-" ANSI_COLOR_RESET "] first_function(): illegal character '%c'!\n", *(userdata+i));
	*(userdata+i) = 'X';
	break;
    }
  }

  // clear userdata2
  memset(userdata2, 0, sizeof(userdata2));
  // prepend
  strncpy(userdata2, (char *)"comment1=cooking%20MCs;userdata=", strlen("comment1=cooking%20MCs;userdata="));
  // user input
  strncat(userdata2, userdata, userdatalength);
  // append
  strncat(userdata2, (char *)";comment2=%20like%20a%20pound%20of%20bacon", strlen(";comment2=%20like%20a%20pound%20of%20bacon"));

  dump(userdata2, strlen(userdata2));
  length = aes_encrypt_cbc(userdata2, ciphertext, strlen(userdata2), key, iv, 16);
}

int is_admin(char *ptr, int length)
{
  int i, plength;
  char decrypted[2048];

  plength = aes_decrypt_cbc(decrypted, ptr, length, key, iv, 16);
  dump(decrypted, plength);

  if(strstr(decrypted, (char *)";admin=true;")==NULL)
  {
    printf("[" ANSI_COLOR_RED "-" ANSI_COLOR_RESET "] is_admin(): you are not admin\n");
    return 0;
  }
  else
  {
    printf("[" ANSI_COLOR_GREEN "+" ANSI_COLOR_RESET "] is_admin(): ADMIN=TRUE!!!\n");
    return 1;
  }
}

int main(void)
{
  char
    userdata[BUFFER_SIZE];
  int
    length,
    ret = 1;

  randomize_key(key, 16);
  randomize_key(iv,  16);

  printf("--- [ first test ] ---\n");
  memset(userdata, 0, sizeof(userdata));
  strncpy(userdata, ";admin=true", strlen(";admin=true"));
  printf(ANSI_COLOR_RED "adversary: trying \"%s\":\n" ANSI_COLOR_RESET, userdata);
  length = first_function(userdata, ciphertext);

  strncpy(userdata, "XadminXtrue", strlen(";admin=true"));
  printf("--- [ second test ] ---\n" ANSI_COLOR_RED "adversary: trying \"%s\":\n" ANSI_COLOR_RESET, userdata);
  length = first_function(userdata, ciphertext);

  printf("--- [ third test ] ---\n" ANSI_COLOR_RED "adversary: modifying ciphertext\n" ANSI_COLOR_RESET);
  *(ciphertext+16) ^= 'X' ^ ';';
  *(ciphertext+22) ^= 'X' ^ '=';
  printf("\n\nciphertext:\n");
  dump(ciphertext, length);

  printf("--- [ result ] ---\n");
  if(is_admin(ciphertext, length)) {
    printf(ANSI_COLOR_GREEN "great success!" ANSI_COLOR_RESET "\n");
    ret = 0;
  }
  return ret;
}
