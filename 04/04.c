#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * line=170 key=53: Now that the party is jumping
 */

int main(void)
{
  int i, j, key, nonascii, lines = 0;
  char line[64], byte[3];
  unsigned char encrypted_char, decrypted[64];
  FILE *fp;

  fp = fopen("./04.txt", "r");
  if(fp == NULL)
  {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  while(fgets(line, sizeof line, fp) != NULL)
  {
    lines++;
    for(key=0, nonascii=0; key<=255; key++, nonascii=0)
    {
      // read 2 bytes at a time since it's ASCII hex
      for(i=0, j=0; i<strlen(line); i+=2, j++)
      {
	// skip newline
	if(line[i]=='\n')
	  continue;
	byte[0] = line[i];
	byte[1] = line[i+1];
	byte[2] = '\0';
	// convert ASCII hex to char
	encrypted_char = strtoul(byte, NULL, 16);
	decrypted[j] = encrypted_char ^ key;
	// printable ASCII + newline
	if( ( (decrypted[j] < 32) && (decrypted[j]!=10) ) || (decrypted[j] >= 127) )
	  nonascii = 1;
      }
      decrypted[j-1] = 0;
      if(!nonascii)
      {
	printf("line=%d key=%d: %s\n", lines, key, decrypted);
      }
    }
  }
  printf("total lines: %d\n", lines);

  fclose(fp);
}
