#include <string.h>
#include <stdio.h>

int main(void)
{
  char string[] = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
  char encrypted_string[148], *ptr;
  char ciphertext[] = "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";
  char key[] = "ICE";
  unsigned char encrypted_char;
  int i;

  ptr = encrypted_string;
  for(i=0; i<strlen(string); i++, ptr+=2)
  {
    encrypted_char = string[i] ^ key[i % strlen(key)];
    sprintf(ptr, "%.2x", encrypted_char);
  }
  *ptr = '\0';
  printf("%s\n", encrypted_string);

  if((strncmp(encrypted_string, ciphertext, 148))==0)
  {
    printf("strings match!\n");
  }
  else
  {
    printf("ERROR: strings don't match!\n");
  }
  return 0;
}
