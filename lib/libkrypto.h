#include <stdio.h>

#define ANSI_COLOR_RED     "\x1b[1;31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_GREEN   "\x1b[1;32m"

extern int pad(unsigned char *ptr, int length, int blocksize);
extern int padding_detect(char *ptr, int strlen, int blocksize);
extern int base64_decode(FILE *fp, char *ptr, int buflen);
extern int base64_encode(char *message, FILE *fp);
extern int randomize_key(char *ptr, int keylen);
extern int encryption_oracle(char *plaintext, char *ciphertext, int length);
extern int encryption_oracle_12(char *plaintext, char *ciphertext, int length);
extern int encryption_oracle_14(char *random_prefix, int random_length, char *plaintext, char *ciphertext, int length);
extern int calculate_hamming_distance_of_byte(unsigned char byte1, unsigned char byte2);
extern int print_bits(unsigned char byte);
extern int calculate_hamming_distance_of_string(char *ptr1, char *ptr2, int length);
extern int aes_decrypt(char *ciphertext, char *plaintext, int ciphertext_length, char *key, int keysize);
extern int aes_encrypt_ecb(char *plaintext, char *ciphertext, int plaintext_length, char *key, int keysize);
extern int aes_encrypt_cbc(char *plaintext, char *ciphertext, int plaintext_length, char *key, char *iv, int keysize);
extern int aes_decrypt_cbc(char *plaintext, char *ciphertext, int ciphertext_length, char *key, char *iv, int keysize);
extern int guess_keysize(char *ciphertext, int ciphertext_length);
extern int guess_ecb_blocksize(char *ciphertext, int ciphertext_length, int *blockpos);
extern int crack_ecb2(char *random_prefix, int random_length, int block_size, char *decrypted, int first_block, int last_block, int alignment);

extern unsigned int _mt19937_index, mt[624];
extern int mt19937_initialize_generator(int seed);
extern unsigned int mt19937_extract_number(void);
extern int mt19937_generate_numbers(void);

/*
typedef struct
{
  int length;
  char *message;
} text;
*/
