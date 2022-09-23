/*
 * 0123456789abcdef
 *
 * original:
 *
 * email=foo@bar.co
 * m&uid=10&role=us
 * er
 *
 * attack:
 *
 * #1:
 *
 * email=foo@bar.co
 * admin&uid=10&rol <- take this block
 * e=user
 *
 * #2:
 *
 * email=foo12@bar.
 * com&uid=10&role=
 * user             <- copy it here
 *
 *
 * email=foo@bar.com&uid=10&role=user
 *
 *
 * 00000280  3f 25 88 f9 66 cb 33 5a 04 41 41 41 41 41 41 41 | ?%..f.3Z.AAAAAAA
 * 00000290  41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 | AAAAAAAAAAAAAAAA
 * 000002a0  41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 | AAAAAAAAAAAAAAAA
 *
 *
 *
 * TODO:
 *   - use strdup
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/libkrypto.h"
#include "../lib/hacking.h"

unsigned char key[16];

char *profile_for(char *email)
{
  int i;
  char *profile;

  // allocate memory for the new profile
  i = strlen("email=") + strlen(email) + strlen("&uid=10&role=user") + 1;
  profile = malloc(i);
  if(profile==NULL)
  {
    printf("ERROR: malloc()\n");
    return -1;
  }

  printf("profile_for():\n  memory allocated=%d\n  email=%s\n", i, email);
  memcpy(profile, (char *)"email=", 6);

  /*
   * Your "profile_for" function should NOT allow encoding metacharacters
   * (& and =). Eat them, quote them, whatever you want to do, but don't
   * let people set their email address to "foo@bar.com&role=admin".
   *
   */
  for(i=0; i<strlen(email); i++)
  {
    switch(*(email+i))
    {
      case '&':
	printf("  illegal character '%c'!\n", *(email+i));
	*(email+i) = 'X';
	break;
      case '=':
	printf("  illegal character '%c'!\n", *(email+i));
	*(email+i) = 'X';
	break;
    }
    *(profile+6+i) = *(email+i);
  }
  strncpy(profile+strlen("email=")+strlen(email), (char *)"&uid=10&role=user", strlen("&uid=10&role=user"));

  return profile;
}

int parse(char *ptr)
{
  char
    *token,
    *ptr2,
    *ptr3,
    key[128],
    value[128];
  int length;

  length = strlen(ptr);
  ptr2 = (char *)malloc(length+1);

  if(ptr2==NULL)
  {
    printf("ERROR: malloc()\n");
    return -1;
  }
  strncpy(ptr2, ptr, length);
  // store ptr2 into ptr3 so we can free it later (as strtok requires making ptr2 -> NULL)
  ptr3 = ptr2;
  printf("parse(): string=\"%s\"\n  length=%d\n", ptr2, length);

  while( (token=strtok(ptr2, "&")) != 0 )
  {
    // In each subsequent call that should parse the same string, str should be NULL.
    if(ptr2!=NULL) {
      ptr2 = NULL;
    }
    printf("  strok(): %s\n", token);

    if(sscanf(token, "%[^=]=%[^=]", key, value)!=2)
    {
      printf("    WARNING: no key=value pair found!");
      continue;
    }
    printf("    key\t\t= %s\n", key);
    printf("    value\t= %s\n", value);
  }

  free(ptr3);
  printf("\n\n");

  return length;
} // int parse(char *ptr)

int main(void)
{
  char parseme[] = "foo=bar&baz=qux&zap=zazzle",
    *profile,
    encrypted_profile[1024],
    decrypted[1024],
    ciphertext_length,
    email[64] = "foo@bar.com";
  int profile_length, decrypt_counter, i;
  unsigned char
    ciphertext_block[16],
    ciphertext_block2[16];

  randomize_key(key, 16);

  printf("TEST ROUND one:\n\n");
  parse(parseme);
  printf("\n// ------------------------------------------------------------\n\n");

  printf("TEST ROUND two:\n\n");
  profile = profile_for(email);
  printf("\n");
  parse(profile);
  free(profile);
  printf("\n// ------------------------------------------------------------\n\n");

  /*
   *
   * adversary
   *
   */
  printf("<ADVERSARY>\n");
  profile = profile_for((char *)"foo@bar.coadmin");
  ciphertext_length = aes_encrypt_ecb(profile, encrypted_profile, 38, (char *)key, 16);
  free(profile);
  memcpy(ciphertext_block, encrypted_profile+16, 16);
  profile = profile_for("foo12@bar.com");
  ciphertext_length = aes_encrypt_ecb(profile, encrypted_profile, 36, (char *)key, 16);
  // actual attack

  // move the last block forward, so we get a valid padding.
  memmove(encrypted_profile+(16*3), encrypted_profile+32, 16);
  // insert the '^admin' block
  memcpy(encrypted_profile+32, ciphertext_block, 16);
  printf("</ADVERSARY>\n");

  i = aes_decrypt(encrypted_profile, decrypted, 16*4, key, 16);

  if(i==-1)
  {
    printf("ERROR: decryption failed!\n");
    return -1;
  }

  // NULL terminate, so we can properly parse it
  *(decrypted+i) = '\0';

  printf("decrypted:\n\n");
  dump(decrypted, i);
  profile_length = parse(decrypted);

  free(profile);

  return 0;
}
