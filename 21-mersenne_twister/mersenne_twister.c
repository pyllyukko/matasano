// https://en.wikipedia.org/wiki/Mersenne_twister#Pseudocode

#include <stdio.h>
#include "../lib/libkrypto.h"

// from http://oeis.org/A221557
unsigned int oeis_values[] = {
  3499211612,
  581869302,
  3890346734,
  3586334585,
  545404204,
  4161255391,
  3922919429,
  949333985,
  2715962298,
  1323567403,
  418932835,
  2350294565,
  1196140740,
  809094426,
  2348838239,
  4264392720,
  4112460519,
  4279768804,
  4144164697,
  4156218106,
  676943009,
  3117454609 };

int main(void)
{
  unsigned int i, j, oeis_value;
  mt19937_initialize_generator(5489); // seed value from OEIS

  printf("testing");
  for(i=0, _mt19937_index=0; i<sizeof(oeis_values)/sizeof(int); i++)
  {
    j = mt19937_extract_number();
    oeis_value = oeis_values[i];
    if(j!=oeis_value)
    {
      printf("TEST FAILED (%u!=%u)!\n", j, oeis_value);
      return 1;
    }
    printf(".");
  }
  printf("test succeeded\n");
}
