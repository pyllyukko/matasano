/*
 * chosen-plaintext attack for the matasano challenge #12
 *
 */
#include <string.h>
#include "../lib/libkrypto.h"

int main(void)
{
  unsigned char
    ciphertext[4096], decrypted[512],
    ciphertext_block[16], chosen_plaintext[64], random_prefix[2048];
  int
    i, j,
    ciphertext_length,
    random_length,
    blockpos,
    initial_block,
    bytepos,
    hamming_distance,
    decrypt_counter,
    padding,
    block_size;

  // random-prefix for the oracle
  random_length = rand() % 512;
  printf("random-prefix length=%d\n", random_length);
  for(i=0; i<random_length; i++)
  {
    *(random_prefix+i) = rand() % 256;
  }

  /* fill chosen_plaintext with A's to get matching ECB blocks
   * run the oracle to determine the block size
   *
   * we also need to align the blocks and find out how many bytes we need to
   * successfully pad the previous block before our chosen plaintext.
   *
   * example:
   *
   * 00000280  3f 25 88 f9 66 cb 33 5a 04 41 41 41 41 41 41 41 | ?%..f.3Z.AAAAAAA
   * 00000290  41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 | AAAAAAAAAAAAAAAA
   * 000002a0  41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 | AAAAAAAAAAAAAAAA
   *
   * we need to pad 7 bytes
   *
   */

  memset(chosen_plaintext, 'A', 64);
  for(i=8; i<=64; i++)
  {
    ciphertext_length = encryption_oracle_14(random_prefix, random_length, chosen_plaintext, ciphertext, i);
    block_size = guess_ecb_blocksize(ciphertext, ciphertext_length, &blockpos);
    if(block_size!=-1)
      break;
  }
  // we need to know how much bytes we need to pad, so we'll have a full chosen plaintext block
  padding = i % block_size;
  ciphertext_length = encryption_oracle_14(random_prefix, random_length, chosen_plaintext, ciphertext, block_size+padding);
  printf("\nsetup:\n  we need to pad %d bytes\n  first chosen plaintext block is #%d\n  ciphertext_length=%d\n  ciphertext_length/block_size=%d\n\n",
         padding, blockpos, ciphertext_length, ciphertext_length/block_size);

  decrypt_counter = crack_ecb2(random_prefix, random_length, block_size, decrypted, blockpos, ciphertext_length/block_size, padding);

  printf("\ndecrypted=%d\n\n", decrypt_counter);
  dump(decrypted, decrypt_counter);

  if(padding < 0 || padding >= block_size)
    printf("WARNING: padding detect error!\n");

  printf("\nDEBUG:\n  random_length=%d\n  plaintext=%d (%d blocks)\n  ciphertext_length=%d\n  padding=%d\n", random_length, ciphertext_length-padding-block_size-random_length, (ciphertext_length-padding-block_size-random_length)/block_size, ciphertext_length, padding);

  return;
}
