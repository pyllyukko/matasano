// https://en.wikipedia.org/wiki/Mersenne_twister#Pseudocode

#include <stdio.h>

unsigned int _index = 0, mt[624];

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

int initialize_generator(int seed)
{
  int i;

  printf("initialize_generator()\n");
  for(i=1, mt[0]=seed; i<624; i++)
  {
    mt[i] = 1812433253 * ( mt[i-1] ^ ( mt[i-1] >> 30 ) ) + i;
  }
}

unsigned int extract_number(void)
{
  unsigned long y;

  if(_index==0)
  {
    generate_numbers();
  }

  y = mt[_index];
  y ^= (y >> 11);
  y ^= (y << 7)		& 2636928640;
  y ^= (y << 15)	& 4022730752;
  y ^= (y >> 18);

  _index = (_index + 1) % 624;

  return y;
}

int generate_numbers(void)
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

int main(void)
{
  unsigned int i, j, oeis_value;
  initialize_generator(5489); // seed value from OEIS

  printf("testing");
  for(i=0, _index=0; i<sizeof(oeis_values)/sizeof(int); i++)
  {
    j = extract_number();
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
