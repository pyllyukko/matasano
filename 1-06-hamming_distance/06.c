#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include "../lib/libkrypto.h"
#include "../lib/hacking.h"

int main(int argc, char *argv[])
{
  char str1[] = "this is a test";
  char str2[] = "wokka wokka!!!";
  int
    i, j,
    hamming_distance = 0,
    keybyte, keypos, keysize, potential_key_bytes, most_potential_keybyte,
    potential_key[256],
    nonascii,
    ciphertext_length = 0,
    probable_keysize,
    blocks,
    alphabets_seen,
    output_mode = 1;
  unsigned char
    decrypt_sample[128],
    ciphertext[4096],
    plaintext[4096],
    key[64],
    ciphertext_sample[2048];
  FILE *fp;

  if (argc != 2) {
    printf("usage: %s filename\n", argv[0]);
    return 1;
  }

  if(strlen(str1)!=strlen(str2))
  {
    printf("error: strings are of unequal size!\n");
  }
  hamming_distance = calculate_hamming_distance_of_string(str1, str2, strlen(str1));
  printf("hamming distance test: %d\n\n", hamming_distance);

  fp = fopen(argv[1], "r");
  if(fp == NULL)
  {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  printf("reading ciphertext into memory:\n");
  ciphertext_length = base64_decode(fp, ciphertext, sizeof(ciphertext));
  fclose(fp);

  if(ciphertext_length==0)
  {
    printf("error: bailing out!\n");
    exit(EXIT_FAILURE);
  }
  printf("  read total %d bytes\n\n", ciphertext_length);

  // print a sample of the ciphertext:
  printf("ciphertext:\n");
  printf("  ");
  dump(ciphertext, 16);
  printf("  ...\n  ");
  dump(ciphertext+ciphertext_length-16, 16);
  printf("\n");

  probable_keysize = guess_keysize(ciphertext, ciphertext_length);

  printf("\nmost probable keysize = %d (%d bits)\n\ncracking the key:\n", probable_keysize, probable_keysize*8);

  blocks = ciphertext_length / probable_keysize;
  printf("  \n  blocks=%d\n\n", blocks);

  if(output_mode==1)
  {
    printf("\033[2J\033[1;1Hcracking key at pos="); /*clear screen*/
  }

  for(keypos=0; keypos<probable_keysize; keypos++)
  {
    if(output_mode==0)
      printf("  keypos=%d\n", keypos);
    else if(output_mode==1)
    {
      printf("\033[1;21H%.2d value=", keypos);
    }

    /* read in ciphertext sample
     *
     * in here, we take every probable_keysize:th byte into an array, try to
     * decrypt that with different key values, count the alphabets and try to
     * figure out the correct key byte value that way.
     */
    for(i=keypos, j=0; i<ciphertext_length; i+=probable_keysize, j++)
      ciphertext_sample[j] = ciphertext[i];

    // for each key byte value
    for(keybyte=0, nonascii=0, alphabets_seen=0, potential_key_bytes=0, most_potential_keybyte=-1;
	keybyte<256;
	keybyte++, nonascii=0, alphabets_seen=0)
    {
      if(output_mode==1)
	printf("\033[1;30H%.2x\n", keybyte);
      // decrypt the sample with the key byte value and count the alphabets
      for(i=0; i<blocks; i++)
      {
        decrypt_sample[i] = ciphertext_sample[i] ^ keybyte;
	if( ((decrypt_sample[i] < 32) && (decrypt_sample[i]!=10)) || (decrypt_sample[i] >= 127) )
	  nonascii = 1;
	/*
	 * detect text inside the decrypted sample.
	 * >='A' && <='z' works also to some extent, but the one we use below
	 * is much better.
	 */
	if(
	   (decrypt_sample[i]>='A'&&decrypt_sample[i]<='Z') ||
	   (decrypt_sample[i]>='a'&&decrypt_sample[i]<='z') ||
	    decrypt_sample[i]==' ' ||
	    decrypt_sample[i]=='\''||
	    decrypt_sample[i]==','||
	    decrypt_sample[i]=='.'
	)
	  alphabets_seen++;
      }

      // print a sample of the text-in-decryption
      if(output_mode==1)
      {
        for(i=0; i<40*16; i++)
        {
	  if(i%probable_keysize==keypos)
	    plaintext[i] = ciphertext[i] ^ keybyte;
	  else if(i%probable_keysize<keypos)
	    plaintext[i] = ciphertext[i] ^ key[i%probable_keysize];
	  else
	    plaintext[i] = ciphertext[i];
        }
	printf("\033[2;1H");
	dump(plaintext, 40*16);
      }

      /*
       * we calculate the amount of alphabets seen in the decrypted output with each key byte value.
       * we also save all the alphabet counts with the key value as index to potential_key[].
       * then we see which value got the most alphabet, and decide that's our most probable
       * key byte value.
       *
       */

      // were we lucky?
      if(!nonascii)
      {
	if(output_mode==0)
	  printf("    possible keybyte=%.2x\talphabets seen=%d\n", keybyte, alphabets_seen);
	key[keypos] = keybyte;
	potential_key_bytes++;
	potential_key[keybyte] = alphabets_seen;
	if(most_potential_keybyte==-1)
	{
	  most_potential_keybyte = keybyte;
	}
	else if(alphabets_seen>potential_key[most_potential_keybyte])
	{
	  if(output_mode==0)
	    printf("      alphabets_seen=%d > potential_key[%.2x]=%d\n",
		   alphabets_seen, most_potential_keybyte, potential_key[most_potential_keybyte]);
	  most_potential_keybyte = keybyte;
	  if(output_mode==0)
	    printf("      most_potential_keybyte=%.2x\n", most_potential_keybyte);
	}
      } // if(!nonascii)
    } // for(keybyte=0 ...

    // this means the guessed keybyte might not be correct
    if(potential_key_bytes>1)
    {
      if(output_mode==0)
	printf("\n    WARNING: several potential key byte values (%d)\n    guessing %.2x\n\n",
	       potential_key_bytes, most_potential_keybyte);
      key[keypos] = most_potential_keybyte;
    }
  } // for(keypos=0; keypos<probable_keysize; keypos++)

  // draw the final output
  if(output_mode==1)
  {
    for(i=0; i<ciphertext_length; i++)
    {
      plaintext[i] = ciphertext[i] ^ key[i % probable_keysize];
    }
    printf("\033[2;1H");
    //dump(plaintext, 600);
    dump(plaintext, ciphertext_length);
  }
  else if(output_mode==0)
  {
    printf("\npossible key:\n");
    dump(key, probable_keysize);

    /*
    hamming_distance = calculate_hamming_distance_of_string(key, "Terminator X: Bring the noise", probable_keysize);
    printf("\n  key: %s\n  hamming distance: %d\n", key, hamming_distance);
    */

    if(output_mode==0)
    {
      printf("\ndecrypted:\n\n");
      for(i=0; i<ciphertext_length; i++)
      {
        printf("%c", ciphertext[i] ^ key[i % probable_keysize]);
      }
    }
  }

  return 0;
}
