// https://en.wikipedia.org/wiki/Mersenne_twister#Pseudocode

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "../lib/libkrypto.h"

int main(void)
{
  unsigned int i, j, random_wait, mt_value;

  srand(time(NULL));
  random_wait = rand() % 961 + 40;

  printf("sleeping\n");
  sleep(random_wait);
  mt19937_initialize_generator(time(NULL));
  random_wait = rand() % 961 + 40;
  printf("sleeping\n");
  sleep(random_wait);
  mt_value = mt19937_extract_number();
  printf("DEBUG: crack_me=%u\n", mt_value);

  // crack it
  printf("cracking");
  for(i=time(NULL)-60*60*24; i<=time(NULL); i++)
  {
    if(i%100==0)
      printf(".");
    mt19937_initialize_generator(i);
    _mt19937_index = 0;
    j = mt19937_extract_number();
    if(j==mt_value)
    {
      break;
    }
  }

  if(j==mt_value)
  {
    printf("\nseed = %u\n", i);
  }
  else
  {
    printf("\nseed not found :(\n");
  }
}
