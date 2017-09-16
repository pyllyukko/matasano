/*
 * chosen-plaintext attack for the matasano challenge #12
 *
 */
#include <string.h>
#include "../lib/libkrypto.h"
#include "../lib/hacking.h"

int main(void)
{
  unsigned char
    ciphertext[1024], decrypted[256],
    chosen_plaintext[64];
  int
    ciphertext_length,
    decrypt_counter,
    block_size;

  // fill chosen_plaintext with A's to get matching ECB blocks
  memset(chosen_plaintext, 'A', 43);
  // run the oracle to determine the block size
  ciphertext_length = encryption_oracle_12((char *)chosen_plaintext, (char *)ciphertext, 43);
  //printf("ciphertext_length = %d\n", ciphertext_length);
  block_size = guess_ecb_blocksize((char *)ciphertext, ciphertext_length, NULL);

  ciphertext_length = encryption_oracle_12((char *)chosen_plaintext, (char *)ciphertext, block_size);
  printf("guessed blocksize=%d\ntotal blocks=%d\n\n", block_size, ciphertext_length/block_size);
  /*
   * here's roughly how this works:
   *   - blockpos is the current cipher block we're trying to crack
   *   - bytepos is the current byte of the current block we're working on
   *   - we use one block of chosen plaintext that we feed to the oracle
   *     and compare the result with chosen plaintext -1 byte that we again
   *     feed to the oracle
   */

  //printf("DEBUG: %d\n", ciphertext_length/block_size);
  decrypt_counter = crack_ecb2(NULL, 0, block_size, (char *)decrypted, 0, ciphertext_length/block_size, 0);

  printf("\ndecrypted=%d\n\n", decrypt_counter);
  dump(decrypted, decrypt_counter);

  return 0;
}
