#include <stdio.h>
#include <string.h>

unsigned char hex[]="\x1b\x37\x37\x33\x31\x36\x3f\x78\x15\x1b\x7f\x2b\x78\x34\x31\x33\x3d\x78\x39\x78\x28\x37\x2d\x36\x3c\x78\x37\x3e\x78\x3a\x39\x3b\x37\x36";

int main(void)
{
  int i, key, nonascii;
  unsigned char decrypted[strlen(hex)+1];
  printf("length: %d\n", strlen(hex));
  for(key=0, nonascii=0; key<=255; key++, nonascii=0)
  {
    for(i=0; i<strlen(hex); i++)
    {
      decrypted[i] = hex[i] ^ key;
      if( (decrypted[i] < 32) || (decrypted[i] >= 127) )
	nonascii = 1;
    }
    decrypted[strlen(hex)] = 0;
    if(!nonascii)
      printf("%d: %s\n", key, decrypted);
  }
}
