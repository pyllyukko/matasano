#include <stdio.h>
#include <string.h>
#include "../lib/libkrypto.h"
#include "../lib/hacking.h"

char
  ciphertext[1024],
  key[16],
  iv[16];

int first_function(char *userdata, char *ciphertext)
{
  char userdata2[1024];
  int i, length;

  // input validation :)
  for(i=0; i<strlen(userdata); i++)
  {
    switch(*(userdata+i))
    {
      case ';':
	printf("first_function(): illegal character '%c'!\n", *(userdata+i));
	*(userdata+i) = 'X';
	break;
      case '=':
	printf("first_function(): illegal character '%c'!\n", *(userdata+i));
	*(userdata+i) = 'X';
	break;
    }
  }

  // clear userdata2
  memset(userdata2, 0, sizeof(userdata2));
  // prepend
  strncpy(userdata2, (char *)"comment1=cooking%20MCs;userdata=", strlen("comment1=cooking%20MCs;userdata="));
  // user input
  strncat(userdata2, userdata, strlen(userdata));
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
    printf("is_admin(): you are not admin\n");
    return 0;
  }
  else
  {
    printf("is_admin(): ADMIN=TRUE!!!\n");
    return 1;
  }
}

int main(void)
{
  char
    userdata[1024];
  int
    length,
    ret = 1;

  randomize_key(key, 16);
  randomize_key(iv,  16);

  memset(userdata, 0, sizeof(userdata));
  strncpy(userdata, ";admin=true", strlen(";admin=true"));
  printf("adversary: trying \"%s\":\n", userdata);
  length = first_function(userdata, ciphertext);

  strncpy(userdata, "XadminXtrue", strlen(";admin=true"));
  printf("adversary: trying \"%s\":\n", userdata);
  length = first_function(userdata, ciphertext);
  *(ciphertext+16) ^= 'X' ^ ';';
  *(ciphertext+22) ^= 'X' ^ '=';
  printf("\n\nciphertext:\n");
  dump(ciphertext, length);

  if(is_admin(ciphertext, length)) {
    printf("great success!\n");
    ret = 0;
  }
  return ret;
}
