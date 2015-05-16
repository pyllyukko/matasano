/*
 * https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher
 *
 */
#include <stdio.h>
#include <string.h>

int main(void)
{
  int
    i, j,
    keyindex,
    alphaindex,
    // for cracking
    keysize, repetition_size, offset, offset1, offset2, blocks, block, distance, distances[256],
    patterns_found = 0, keysize_candidates[256], keysize_match;
  char
    /*
    plaintext[]	= "ATTACKATDAWN",
    key[]	= "LEMON",
    */
    /*
    plaintext[]	= "ALLISWELL",
    key[]	= "CIPHER",
    */
    plaintext[] = "CRYPTOISSHORTFORCRYPTOGRAPHY",
    key[]	= "ABCD",
    //key[]	= "CIPHERCOD",
    /*
    plaintext[] = "BASEDONPREVIOUSWORMSTHATUSUALLYMEANSTHEREAREATLEASTTIMESASMANYINFECTED",
    key[] = "DCBDCB",
    */
    //ciphertext[] = "VHVSSPQUCEMRVBVBBBVHVSURQGIBDUGRNICJQUCERVUAXSSR",
    ciphertext[] = "CSASTPKVSIQUTGQUCSASTPIUAQJB",
    //ciphertext[] = "YVZUXNAVYIBWCNKNJVASZAFTIUZRKRYSOVXTKFTOBWKJCTINKPQDIQFFPCAJNBUVLFHYCZNBXGBYFRESSHVPXYVRVMXOJEMNARMIXHYKWTBVOSWSJQPAOSNINHNQYQRSGWTZMCFQSNKXMCLVXMOKDYYKVYMQZDCOJGLTIYCXKZJZFVWTTKGBYXLUXYCDCOJGLTIYVRVAJQRPLFBUNINHZYSSRVTLTKDWGMKZNPGSIGUZWIGHQSKUTZHKJIROXKGUVICWJTFTXMSEGBYZVXIQWEVLJAZFHQSIKKMHYGVJARIMHKRPHTYEQARCIGXJLKVSROBGWZFVVLFHZVMXGFNZFPCGMXVFWPIDIQFFPCANZGKRYYWEUSRSEWVXSIAVMMDGSWGFOIYVZPKROIALFRRNMYHCGPFASNMYHCGPFASNMYHCGPFASOEWMYCHFZZVXQSCCQGKYQWJTCGIESNCWBVZVIFGJPSBWUQRYKRPXYCDCOJHYKWMOIFIWHYCRNHEGIIGKQFJWWASZJVUSQJVFEQCKQJXWDRPJQIATYCTJEQZVPKJGPQYUFFDEGZPCPWSRFCMOMGXMSTQHJOEFANZCDVJSQGVNUYVXMFFWKMWKKJNHYGPUGDQWYCWVLJDCCMSHVZXNGJVEYWTCXJOTJSKHYGPJJVNWNAEQXFARUSHVZUXYVVHYSBPVLNBXKWYVRVHJDVPHNBXQRBVZELWOEFSRYVACTIXGXYVRVTTSDOMLVKDIJLREXQMKJIWWXJXTTWUIYHFUYHQVUWKICNCRCLPXFBRVXFQBYIQZJGIQWKVPJPZVQTFVNMYHCGFNHDQVJHYGVJ",
    chars[256],
    *ptr, *ptr1, *ptr2;

  // print the table
  printf("  ");
  for(j='A', i=0; j<='Z'; j++, i++)
  {
    chars[i] = j;
    printf("%c", j);
  }
  printf("\n\n");

  for(i=0; i<=25; i++)
  {
    printf("%c ", chars[i]);
    for(j=0; j<=25; j++)
    {
      printf("%c", chars[(j+i)%26]);
    }
    printf("\n");
  }
  // </table>

/*

  // encrypt
  for(i=0; i<strlen(plaintext); i++)
  {
    keyindex		= key[i%strlen(key)]-65;
    alphaindex		= plaintext[i]-65;
    ciphertext[i]	= chars[(keyindex+alphaindex)%26];
  }
  ciphertext[i] = '\0';

  printf("\nplaintext\t= %s\nkey\t\t= ", plaintext);

  for(i=0; i<strlen(plaintext); i++)
  {
    printf("%c", key[i%strlen(key)]);
  }
  printf("\n");
  printf("ciphertext\t= %s\n", ciphertext);

  printf("\ndecrypted\t= ");
  for(i=0; i<strlen(plaintext); i++)
  {
    keyindex	= key[i%strlen(key)]-65;
    alphaindex	= (ciphertext[i]-65-keyindex+26)%26;
    printf("%c", chars[alphaindex]);
  }
  printf("\n");

  */

  /*
   * CSASTPKVSIQUTGQUCSASTPIUAQJB
   *
   * *VHVS*SP*QUCE*MRVBVBBB*VHVS*URQGIBDUGRNICJ*QUCE*RVUAXSSR
   *
   *
   *
   *
   *
   */

  //repetition_size = strlen(ciphertext)-3;
  for(repetition_size=strlen(ciphertext)/2; repetition_size>=3; repetition_size--)
  {
    printf("trying repetition_size %d\n", repetition_size);
    /*
    offset1 = 0;
    while(1)
    {
      printf("  searching from offset %d\n", offset1);
      offset1++;
      if(offset1 + repetition_size > strlen(ciphertext)-repetition_size)
      //else if(offset1 + repetition_size > strlen(ciphertext)/2)
	break;
    }
    */
    /*
     * find patterns in ciphertext.
     *
     * this is still kinda unoptimized, since this matches all patterns, e.g.:
     *   "CSASTP", "SASTP", "ASTP" & etc. but the distances still match, so we don't care.
     */
    for(offset1=0;
	offset1 + repetition_size <= strlen(ciphertext)-repetition_size;
	offset1++)
    {
      ptr1 = ciphertext+offset1;
      printf("  searching from offset %d\n", offset1);
      for(offset2=offset1+repetition_size; offset2<=strlen(ciphertext)-repetition_size; offset2++)
      {
	ptr2 = ciphertext+offset2;
        //printf("    searching from %d\n", offset2);
	if(strncmp(ptr1, ptr2, repetition_size)==0)
	{
	  distance = ptr2 - ptr1;
	  /*
	  printf("FOUND! distance=%d\n", distance);
	  printf("pattern found=%s\n", ptr2);
	  */
	  distances[patterns_found++] = distance;
	  /*
	  for(i=0; i<repetition_size; i++)
	  {
	    printf("%c", *ptr1+i);
	  }
	  printf("\n");
	  */
	  break;
	}
      }
    }
  }
  printf("\npossible key sizes:\n");
  for(keysize=50, keysize_match=1;
      keysize>2;
      keysize--, keysize_match=1)
  {
    // go through all the distances
    for(i=0, keysize_candidates[i] = 0;
	i<patterns_found;
	i++, keysize_candidates[i] = 0)
    {
      distance = distances[i];
      if(distance%keysize!=0)
      {
	continue;
      }
      keysize_candidates[i] = 1;
    }

    // check that this keysize matches all distances
    for(i=0; i<patterns_found; i++)
    {
      // all keysizes must match
      if(keysize_candidates[i]==0)
      {
        keysize_match = 0;
	break;
      }
    }
    if(keysize_match)
      printf("  possible keysize=%d\n", keysize);
  }
}
