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
  //strncpy(profile, (char *)"email=", strlen("email="));
  //strncat(profile, (char *)"email=", strlen("email="));

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
  //strncat(profile+strlen("email="), email, strlen(email));
  //strncat(profile+strlen("email="), email, strlen(email));
  strncpy(profile+strlen("email=")+strlen(email), (char *)"&uid=10&role=user", strlen("&uid=10&role=user"));

  //dump(profile, 50);

  return profile;
}

int parse(char *ptr)
{
  char
    *token,
    *ptr2,
    *ptr3,
    /*
    *token2,
    *rightside,
    *leftside,
    */
    key[128],
    value[128];
  int length;

  length = strlen(ptr);
  ptr2 = (char *)malloc(length+1);
  /*
  printf("memory from malloc():\n");
  dump(ptr2, 50);
  printf("\n");
  */
  if(ptr2==NULL)
  {
    printf("ERROR: malloc()\n");
    return -1;
  }
  strncpy(ptr2, ptr, length);
  //dump(ptr2, 50);


  // TEST

  /*
  ptr3 = malloc(50000);
  if(ptr3==NULL)
  {
    printf("ERROR: malloc()\n");
    return -1;
  }
  free(ptr3);
  */

  printf("parse(): string=\"%s\"\n  length=%d\n", ptr2, length);

  //printf("DEBUG: ptr2 before=%d\n", ptr2);
  token = strtok(ptr2, "&");
  //printf("DEBUG: ptr2 after=%d\n", ptr2);

  printf("  strok(): %s\n", token);

  sscanf(token, "%[^=]=%[^=]", key, value);
  printf("    key\t\t= %s\n", key);
  printf("    value\t= %s\n", value);

  /*
   * strtok() sucks! it replaces the delimeter with NULLs.
   *
   * maybe we should copy the whole string to our own buffer and process that,
   * or then just use sscanf for everything.
   *
   */

  while( (token=strtok(NULL, "&")) != 0 )
  {
    printf("  strok(): %s\n", token);

    /*
    leftside = strtok_r(token, "=", &rightside);
    printf("    key\t\t= %s\n    value\t= %s\n", leftside, rightside);
    */

    if(sscanf(token, "%[^=]=%[^=]", key, value)!=2)
    {
      printf("    WARNING: no key=value pair found!");
      continue;
    }
    //printf("    sscanf()\t= %d\n", i);
    printf("    key\t\t= %s\n", key);
    printf("    value\t= %s\n", value);

    /*
    token2 = strtok(token, "=");
    printf("    key\t\t= %s\n", token2);
    token2 = strtok(NULL, "=");
    printf("    value\t= %s\n", token2);
    */

    // In each subsequent call that should parse the same string, str should be NULL.

  }

  /*
   * WTF?!?
   *
   * *** glibc detected *** ./13: free(): invalid next size (fast): 0x0000000001e95700 ***
   * ======= Backtrace: =========
   * [0x42d022]
   * [0x431b77]
   * [0x4005a2]
   * [0x400979]
   * [0x4175ee]
   * [0x400329]
   * ======= Memory map: ========
   * 00400000-004b6000 r-xp 00000000 fe:01 1314678                            /home/jk/private/matasano/13/13
   * 006b5000-006c6000 rw-p 000b5000 fe:01 1314678                            /home/jk/private/matasano/13/13
   * 006c6000-006c9000 rw-p 00000000 00:00 0 
   * 01e93000-01eb6000 rw-p 00000000 00:00 0                                  [heap]
   * 7fac7c000000-7fac7c021000 rw-p 00000000 00:00 0 
   * 7fac7c021000-7fac80000000 ---p 00000000 00:00 0 
   * 7fac82750000-7fac82751000 rw-p 00000000 00:00 0 
   * 7fff6eb09000-7fff6eb1e000 rw-p 00000000 00:00 0                          [stack]
   * 7fff6eba3000-7fff6eba4000 r-xp 00000000 00:00 0                          [vdso]
   * ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0                  [vsyscall]
   * Aborted
   *
   */

  //free(ptr2);
  //printf("DEBUG: ptr2=%d\n", ptr2);

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
  //dump(profile, 50);
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
  memcpy(ciphertext_block, encrypted_profile+16, 16);
  profile = profile_for("foo12@bar.com");
  //ciphertext_length = aes_encrypt_ecb(profile, encrypted_profile, 50, (char *)key, 16);
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

  //printf("\nfull profile: %s\n", profile);

  /*
  profile_length = parse(profile);
  dump(profile, profile_length);
  */

  profile_length = parse(decrypted);
  //profile_length = parse(decrypted);
  dump(decrypted, profile_length);

  free(profile);

  return 0;
}
