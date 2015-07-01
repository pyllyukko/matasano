#include <stdio.h>
#include <openssl/bio.h>	// for base64 decode
#include <openssl/evp.h>	// -- || --
#include <openssl/aes.h>
#include "hacking.h"
#include "libkrypto.h"

#define ECB_MODE 0
#define CBC_MODE 1

unsigned char oracles_key[16] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
int oracles_key_initialized = 0;

/*
 * PKCS#7 padding
 *
 * https://en.wikipedia.org/wiki/Block_cipher_modes_of_operation#Padding
 *
 */
int pad(unsigned char *ptr, int length, int blocksize)
{
  int i, j;
  unsigned char padding;

  padding = blocksize-(length%blocksize);
  //printf("pad(): padding plaintext string with %d bytes\n", padding);

  for(i=0; i<padding; i++)
    *(ptr+length+i) = padding;

  return padding;
}

/*
 * PKCS#7 padding
 *
 * this will return the size of the padding or -1 if there are some errors
 */
int padding_detect(char *ptr, int strlen, int blocksize)
{
  int padding, i;

  padding = *(ptr+strlen-1);

  /*
   * https://tools.ietf.org/html/rfc5652#section-6.3
   * "including input values that are already a multiple of the block size"
   *
   */
  if(padding>blocksize)
  {
    printf("padding_detect(): INVALID PADDING: padding=%d > blocksize=%d\n", padding, blocksize);
    return -1;
  }

  printf("padding_detect(): padding=%d strlen=%d blocksize=%d\n", padding, strlen, blocksize);

  /*
  for(i=0; i<strlen; i++)
  {
    printf("  padding_detect(): i=%d %c (0x%.2x)\n", i, *(ptr+i), *(ptr+i));
  }
  */
  //printf("\n  padding_detect(): validate\n");
  for(i=strlen-1; i>=strlen-padding; i--)
  {
    printf("  i=%d 0x%.2x\n", i, *(ptr+i));

    if(*(ptr+i)!=padding)
    {
      printf("padding_detect(): INVALID PADDING!\n");
      return -1;
    }
  }
  printf("  valid padding\n");
  return padding;
}

int base64_decode(FILE *fp, char *ptr, int buflen)
{
  BIO *bio, *b64, *bio_out;
  char inbuf[512];
  int inlen, total_length=0, i, j=0;

  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new_fp(fp, BIO_NOCLOSE);
  bio = BIO_push(b64, bio);
  //printf("base64_decode(): decoding:\n");
  while((inlen = BIO_read(bio, inbuf, 512)) > 0)
  {
    //printf("  read %d bytes\n", inlen);
    if(j+inlen>buflen)
    {
      printf("base64_decode(): ERROR: output buffer not big enough (%d > %d)!\n", j+inlen, buflen);
      return -1;
    }
    for(i=0; i<inlen; i++)
    {
      *(ptr+j++) = inbuf[i];
    }
    /*
    for(i=ciphertext_length, j=0; j<inlen; i++, j++)
    {
      ciphertext[i] = inbuf[j];
    }
    */
    total_length+=inlen;
  }
  //printf("  read total %d bytes (%d blocks)\n\n", ciphertext_length, ciphertext_length/keysize);
  BIO_free_all(bio);

  return total_length;
}

int base64_encode(char *message, FILE *fp)
{
  BIO *bio, *b64;
  //char message[] = "Hello World \n";
  
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new_fp(fp, BIO_NOCLOSE);
  bio = BIO_push(b64, bio);
  BIO_write(bio, message, strlen(message));
  BIO_flush(bio);
  
  BIO_free_all(bio);
}

int randomize_key(char *ptr, int keylen)
{
  int i;
  printf("randomize_key(): RAND_MAX is %u\n", RAND_MAX);
  // seed
  srand(time(0));

  for(i=0; i<keylen; i++)
  {
    *(ptr+i) = rand()%256;
  }
  /*
  printf("random values from 0 to RAND_MAX\n");
  for(i=0; i < 8; i++)
    printf("%d\n", rand());
  printf("random values from 1 to 20\n");
  for(i=0; i < 8; i++)
    printf("%d\n", (rand()%20)+1);
  */

}

/*
 * challenge #11
 *
 */
int encryption_oracle(char *plaintext, char *ciphertext, int length)
{
  unsigned char iv[16], key[16], plaintext2[length+20];
  int i, j;

  srand(time(0));
  randomize_key(key, 16);

  printf("encryption_oracle()\n");
  // APPEND random bytes
  for(i=0; i<rand()%6+5; i++)
  {
    *(plaintext2+i) = rand()%256;
  }
  printf("  appended %d random bytes:\n    ", i);
  dump(plaintext2, i);

  for(j=0; j<length; j++)
  {
    *(plaintext2+i++) = *(plaintext+j);
  }

  // PREPEND random bytes
  for(j=0; j<rand()%6+5; j++)
  {
    *(plaintext2+i++) = rand()%256;
  }
  printf("  prepended %d random bytes:\n    ", j);
  dump(plaintext2+i-j, j);

  printf("  %d bytes added (total length=%d, real plaintext=%d)\n", i-length, i, length);

  switch(rand()%2)
  {
    case ECB_MODE:
      i = aes_encrypt_ecb(plaintext2, ciphertext, i, (char *)key, 16);
      printf("encryption_oracle(): choose \033[1mECB\033[0m, ciphertext length with padding=%d\n", i);
      break;
    case CBC_MODE:
      i = aes_encrypt_cbc(plaintext2, ciphertext, i, (char *)key, (char *)iv, 16);
      printf("encryption_oracle(): choose \033[1mCBC\033[0m, ciphertext length with padding=%d\n", i);
      break;
  }

  return i;
}

/*
 * challenge #12
 *
 */
int encryption_oracle_12(char *plaintext, char *ciphertext, int length)
{
  unsigned char plaintext2[512];
  unsigned char *key;
  int i, j;
  FILE *fp;

  key = (unsigned char *)&oracles_key;

  // create the key if not done already
  //if(memcmp(oracles_key, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16)==0)
  if(!oracles_key_initialized)
  {
    randomize_key(oracles_key, 16);
    //memset(oracles_key, 'A', 16);
    oracles_key_initialized=1;
  }

  /*
  for(i=0; i<length; i++)
  {
    *(plaintext2+i) = *(plaintext+i);
    //printf("encryption_oracle_12(): chosen plaintext=%.2x\n", *(plaintext2+i));
  }
  */
  memcpy(plaintext2, plaintext, length);
  //printf("encryption_oracle_12(): %d bytes\n", i);

  fp = fopen("./12.txt", "r");
  //fp = fopen("./tanelin_viesti.txt", "r");
  if(fp == NULL)
  {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  //printf("encryption_oracle_12(): appending magic plaintext\n");
  //printf("encryption_oracle_12(): DEBUG: sizeof(plaintext2)=%d\n", sizeof(plaintext2));
  j = base64_decode(fp, plaintext2+length, sizeof(plaintext2));
  fclose(fp);

  //aes_encrypt_ecb(plaintext, ciphertext, length, (char *)key, 16);
  i = aes_encrypt_ecb(plaintext2, ciphertext, length+j, (char *)key, 16);
  //printf("encryption_oracle_12(): plaintext length=%d with padding=%d\n", length+j, i);

  return i;
} // encryption_oracle_12()

/*
 * challenge #14
 *
 */
int encryption_oracle_14(char *random_prefix, int random_length, char *plaintext, char *ciphertext, int length)
{
  unsigned char plaintext2[2048];
  unsigned char *key = oracles_key;
  int i, j;
  FILE *fp;

  memcpy(plaintext2,			random_prefix,	random_length);
  memcpy(plaintext2+random_length,	plaintext,	length);

  fp = fopen("./12.txt", "r");
  //fp = fopen("./tanelin_viesti.txt", "r");
  if(fp == NULL)
  {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  //printf("encryption_oracle_14(): appending magic plaintext\n");
  //printf("encryption_oracle_14(): DEBUG: sizeof(plaintext2)=%d\n", sizeof(plaintext2));
  j = base64_decode(fp, plaintext2+random_length+length, sizeof(plaintext2));
  fclose(fp);

  if(j!=138)
    printf("encryption_oracle_14(): WARNING: plaintext length=%d\n", j);

  // for debugging
  //dump(plaintext2, random_length+length+j);

  // TODO: FIX BUG!!! sometimes the count is wrong!!

  //aes_encrypt_ecb(plaintext, ciphertext, length, (char *)key, 16);
  i = aes_encrypt_ecb(plaintext2, ciphertext, random_length+length+j, (char *)key, 16);
  //printf("encryption_oracle_14(): plaintext length=%d with padding=%d\n", length+j, i);

  //printf("encryption_oracle_14():\n  random-prefix=%d\n  chosen plaintext=%d\n  plaintext=%d\n  total=%d vs. %d\n", random_length, length, j, i, length+random_length+j);

  // for debugging

  /*
  printf("encryption_oracle_14()\n");
  dump(plaintext2, i);
  */

  return i;
} // encryption_oracle_14()

int calculate_hamming_distance_of_byte(unsigned char byte1, unsigned char byte2)
{
  int i, hamming_distance=0, bit1, bit2;
  for(i=7; i>=0; i--)
  {
    bit1 = byte1 >> i & 1;
    bit2 = byte2 >> i & 1;
    if(bit1!=bit2)
    {
      hamming_distance++;
    }
  }

  return hamming_distance;
}

int print_bits(unsigned char byte)
{
  int i;

  for(i=7; i>=0; i--)
  {
    printf("%d ", byte >> i & 1);
    //printf("%c ", (byte & (1 << i)) ? '1' : '0' );
  }
  printf("\n");

}

int calculate_hamming_distance_of_string(char *ptr1, char *ptr2, int length)
{
  int i, j, hamming_distance=0, bit1, bit2;
  unsigned char byte1, byte2;
  for(i=0; i<length; i++)
  {
    byte1 = *ptr1;
    byte2 = *ptr2;
    for(j=7; j>=0; j--)
    {
      bit1 = byte1 >> j & 1;
      bit2 = byte2 >> j & 1;
      if(bit1!=bit2)
      {
        hamming_distance++;
      }
    }
    ptr1++;
    ptr2++;
  }

  return hamming_distance;
}

/*
 * returns ciphertext_length - padding
 */
int aes_decrypt(char *ciphertext, char *plaintext, int ciphertext_length, char *key, int keysize)
{
  AES_KEY dec_key;
  int i, padding;

  AES_set_decrypt_key(key, keysize*8, &dec_key);
  printf("\naes_decrypt(): decrypting %d bytes with %d bit key", ciphertext_length, keysize*8);
  for(i=0; i<ciphertext_length; i+=keysize)
  {
    AES_decrypt(ciphertext+i, plaintext+i, &dec_key);
    printf(".");
  }
  printf("\n\n");
  padding = padding_detect(plaintext, ciphertext_length, keysize);
  if(padding==-1)
  {
    return -1;
  }

  return ciphertext_length-padding;
}

int aes_encrypt_ecb(char *plaintext, char *ciphertext, int plaintext_length, char *key, int keysize)
{
  AES_KEY enc_key;
  int i, padding;

  padding = pad(plaintext, plaintext_length, keysize);
  AES_set_encrypt_key(key, keysize*8, &enc_key);
  // TODO: this is useful in some levels, but at least not in 12 (and maybe 14)
  /*
  printf("\naes_encrypt_ecb(): encrypting %d bytes with %d bit key, padding=%d, ciphertext length=%d\n",
         plaintext_length, keysize*8, padding, plaintext_length+padding);
  */
  for(i=0; i<plaintext_length+padding; i+=keysize)
  {
    AES_encrypt(plaintext+i, ciphertext+i, &enc_key);
  }
  if(padding==-1)
  {
    return -1;
  }

  return plaintext_length + padding;
}

/*
 * https://en.wikipedia.org/wiki/Cipher_block_chaining
 *
 * used in #10, #11 & #16
 *
 */
int aes_encrypt_cbc(char *plaintext, char *ciphertext, int plaintext_length, char *key, char *iv, int keysize)
{
  AES_KEY enc_key;
  int i, j, padding;

  /*
   * IV
   */
  for(i=0; i<keysize; i++)
  {
    *(plaintext+i) = *(plaintext+i) ^ *(iv+i);
  }

  // pad the plaintext data
  padding = pad(plaintext, plaintext_length, keysize);
  AES_set_encrypt_key(key, keysize*8, &enc_key);
  // TODO: this is useful in some levels, but at least not in 12 (and maybe 14)
  printf("\naes_encrypt_cbc(): encrypting %d bytes with %d bit key, padding=%d, ciphertext length=%d\n",
         plaintext_length, keysize*8, padding, plaintext_length+padding);
  // i = block offset
  for(i=0; i<plaintext_length+padding; i+=keysize)
  {
    if(i<keysize)
    {
      AES_encrypt(plaintext+i, ciphertext+i, &enc_key);
    }
    else
    {
      for(j=0; j<keysize; j++)
      {
	*(plaintext+i+j) = *(plaintext+i+j) ^ *(ciphertext+i+j-keysize);
      }
      AES_encrypt(plaintext+i, ciphertext+i, &enc_key);
    }
  }
  if(padding==-1)
  {
    return -1;
  }

  return plaintext_length + padding;
} // aes_encrypt_cbc()

int aes_decrypt_cbc(char *plaintext, char *ciphertext, int ciphertext_length, char *key, char *iv, int keysize)
{
  AES_KEY dec_key;
  int
    i, j, padding;

  AES_set_decrypt_key(key, keysize*8, &dec_key);
  // i = block
  for(i=0; i<ciphertext_length; i+=keysize)
  {
    if(i<keysize)
    {
      printf("decrypting first block\n");
      AES_decrypt(ciphertext+i, plaintext+i, &dec_key);
      // remove IV

      /*
      for(i=0; i<keysize; i++)
      {
	*(plaintext+i) = *(plaintext+i) ^ *(iv+i);
      }
      */
    }
    else
    {
      AES_decrypt(ciphertext+i, plaintext+i, &dec_key);
      for(j=0; j<keysize; j++)
      {
        *(plaintext+i+j) = *(plaintext+i+j) ^ *(ciphertext+i+j-keysize);
      }
    }
  }
  /*
   * i'm not entirely sure these are in the right order :)
   *
   */
  padding = padding_detect(plaintext, ciphertext_length, keysize);
  for(i=0; i<keysize; i++)
  {
    *(plaintext+i) = *(plaintext+i) ^ *(iv+i);
  }

  return ciphertext_length-padding;
} // aes_decrypt_cbc()

int guess_keysize(char *ciphertext, int ciphertext_length)
{
  int
    i,
    max_keysize = 40,
    block, blocks,
    hamming_distance, total_hamming_distance,
    probable_keysize, keysize;
  float
    normalized_hamming_distance,
    lowest_hamming_distance;
  unsigned char
    keysize_test[2][max_keysize];

  printf("guess_keysize(): guessing keysize:\n");
  /*
   * try key/block sizes between 2-max_keysize
   *
   */
  for(keysize=2, lowest_hamming_distance=-1;
      keysize<=max_keysize;
      keysize++)
  {
    blocks = ciphertext_length / keysize;
    total_hamming_distance = 0;
    for(block=0; block<blocks; block++)
    {
      /*
       * build an array the length of the assumed keysize
       */
      for(i=0; i<keysize; i++)
      {
        keysize_test[0][i] = ciphertext[i+( block * keysize )];
        keysize_test[1][i] = ciphertext[i+( block * keysize ) + keysize];
      }
      hamming_distance = calculate_hamming_distance_of_string(keysize_test[0], keysize_test[1], keysize);
      total_hamming_distance+=hamming_distance;
      normalized_hamming_distance = (float)(total_hamming_distance/blocks)/keysize;
    }
    printf("  keysize=%d\tblocks=%d\ttotal hamming distance=%d\tnormalized total hamming distance=%.1f\n",
	   keysize, blocks, total_hamming_distance, normalized_hamming_distance);
    if(lowest_hamming_distance==-1)
    {
      lowest_hamming_distance = normalized_hamming_distance;
      probable_keysize = keysize;
    }
    else if(normalized_hamming_distance<lowest_hamming_distance)
    {
      lowest_hamming_distance = normalized_hamming_distance;
      probable_keysize = keysize;
    }
  }

  return probable_keysize;
} // guess_keysize()

int guess_ecb_blocksize(char *ciphertext, int ciphertext_length, int *blockpos)
{
  int
    i,
    block1, block2, blocks,
    blocksize;
  unsigned char
    blocksize_test[2][512];

  printf("guess_ecb_blocksize(): guessing blocksize:\n");
  /*
   * try key/block sizes between 40-3
   *
   * from high to low, so the biggest matching block should win :)
   *
   *
   * ... 2 is already too small block size. we find matching blocks always.
   *     noticed this on #14.
   */
  for(blocksize=40; blocksize>=3; blocksize--)
  {
    blocks = ciphertext_length / blocksize;

    /*
     * check if it's divisible
     *
     */
    if(ciphertext_length%blocksize!=0)
      continue;
    printf("  blocksize=%d blocks=%d\n", blocksize, blocks);
    /*
     * read first comparison block
     */
    for(block1=0; block1<blocks; block1++)
    {
      memcpy(blocksize_test[0], ciphertext + (block1*blocksize), blocksize);
      /*
       * read second comparison block
       */
      for(block2=0; block2<blocks; block2++)
      {
	/*
	 * no point to compare the same block against itself
	 *
	 */
	if(block1==block2)
	  continue;

	memcpy(blocksize_test[1], ciphertext + (block2*blocksize), blocksize);
	if(memcmp(blocksize_test[0], blocksize_test[1], blocksize)==0)
	{
	  printf("    FOUND MATCHING BLOCKS %d==%d!\n", block1, block2);
	  if(blockpos!=NULL)
	    *blockpos = block1;
	  return blocksize;
	}
      } // for(block2=0; block2<blocks; block2++)
    } // for(block1=0; block1<blocks; block1++)
  } // for(blocksize=40, lowest_hamming_distance = -1; blocksize>=3; blocksize--)

  // this means we failed to find two matching blocks
  return -1;
} // guess_ecb_blocksize()

/*
 * this is used for both #12 & #14
 * #12 doesn't use random_* and the alignment is 0
 *
 * TODO: make it so, that this function does not know the key. only the oracle.
 *
 */
int crack_ecb2(char *random_prefix, int random_length, int block_size, char *decrypted, int first_block, int last_block, int alignment)
{
  int
    i, j,		// j = blockpos starting from zero!!!
    ciphertext_length,
    blockpos,
    bytepos,
    decrypt_counter;
  unsigned char
    ciphertext[4096], ciphertext_block[16], chosen_plaintext[64];
  /*
   * here's roughly how this works:
   *   - blockpos is the current cipher block we're trying to crack
   *   - first_block is our chosen plaintext block
   *   - bytepos is the current byte of the current block we're working on
   *   - we use one block of chosen plaintext that we feed to the oracle
   *     and compare the result with chosen plaintext -1 byte that we again
   *     feed to the oracle
   */
  printf("crack_ecb2(): cracking %d blocks from block %d with %d byte alignment:\n", last_block-first_block-1, first_block, alignment);
  //for(blockpos=0, decrypt_counter=0; blockpos<ciphertext_length/block_size; blockpos++)
  for(blockpos=first_block, j=0, decrypt_counter=0;
      blockpos<last_block-1;
      blockpos++, j++)
  {
    printf("  block %d\n", blockpos);
    // fill the chosen plaintext array
    if(blockpos==first_block)
    {
      memset(chosen_plaintext, 'A', block_size-1+alignment);
    }
    else if(decrypt_counter>=block_size)
    {
      /* take the plaintext from the last cracked block
       * shift 1 byte left (+1)
       */
      //printf("crack_ecb2(): DEBUG: decrypt_counter=%d\n", decrypt_counter);
      memcpy(chosen_plaintext+alignment, decrypted+(block_size*(j-1))+1, block_size-1);
    }
    else if(decrypt_counter<block_size)
    {
      // something went screwey
      printf("crack_ecb2(): WARNING: we've advanced beyond the first block, but not succeeded to decrypt it!");
    }

    // crack the block from the end to the beginning
    for(bytepos=block_size-1; bytepos>=0; bytepos--)
    {
      // we try each byte value
      for(i=0; i<256; i++)
      {
	// change the last byte of the chosen plaintext block
	*(chosen_plaintext + alignment + block_size - 1) = i;
	//dump(chosen_plaintext, block_size+alignment);

	// feed the chosen plaintext to the oracle
	if(random_length==0)
	  ciphertext_length = encryption_oracle_12(chosen_plaintext, ciphertext, block_size+alignment);
	else
	  ciphertext_length = encryption_oracle_14(random_prefix, random_length, chosen_plaintext, ciphertext, block_size+alignment);

	// store the ciphertext (from chosen plaintext) for comparison
	memcpy(ciphertext_block, ciphertext+(first_block*block_size), block_size);

	// call the oracle again
	if(random_length==0)
	  ciphertext_length = encryption_oracle_12(chosen_plaintext, ciphertext, bytepos+alignment);
	else
	  ciphertext_length = encryption_oracle_14(random_prefix, random_length, chosen_plaintext, ciphertext, bytepos+alignment);

	// if the ciphertext blocks match, we've succeeded to decrypt one (more) byte
	if(memcmp(ciphertext_block, ciphertext+(blockpos*block_size), block_size)==0)
	{
	  // write down the decrypted byte
	  *(decrypted+decrypt_counter++) = i;
	  /* shift left by one byte
	   * ...in the beginning of the for loop, we again replace the last byte
	   */
	  memmove(chosen_plaintext+alignment, chosen_plaintext+alignment+1, block_size-1);
	  break;
	}
      } // for(i=0; i<256; i++)
    } // for(bytepos=block_size-1; bytepos>=0; bytepos--)
    printf("    ");
    // display the full decrypted block
    dump(decrypted+(j*block_size), block_size);
  } // for(blockpos=0, decrypt_counter=0; blockpos<ciphertext_length/block_size; blockpos++)

  return decrypt_counter-1;
} // int crack_ecb2()

/*
 * mersenne twister functions
 *
 */

unsigned int _mt19937_index = 0, mt[624];

int mt19937_initialize_generator(int seed)
{
  int i;

  for(i=1, mt[0]=seed; i<624; i++)
  {
    mt[i] = 1812433253 * ( mt[i-1] ^ ( mt[i-1] >> 30 ) ) + i;
  }
}

unsigned int mt19937_extract_number(void)
{
  unsigned long y;

  if(_mt19937_index==0)
  {
    mt19937_generate_numbers();
  }

  y = mt[_mt19937_index];
  y ^= (y >> 11);
  y ^= (y << 7)		& 2636928640;
  y ^= (y << 15)	& 4022730752;
  y ^= (y >> 18);

  _mt19937_index = (_mt19937_index + 1) % 624;

  return y;
}

int mt19937_generate_numbers(void)
{
  unsigned int i, y;

  for(i=0; i<624; i++)
  {
    y = ( mt[i] & 0x80000000 ) + ( mt[(i+1) % 624] & 0x7fffffff );
    mt[i] = mt[(i + 397) % 624] ^ (y >> 1);
    if(y % 2 != 0) // y is odd
    {
      mt[i] = mt[i] ^ 2567483615; // 0x9908b0df
    }
  }
}
