libdir=lib
bins=1-03/03 1-04/04 1-05/vigenere 1-05/05 1-06-hamming_distance/06 1-07/07 1-07/encrypt 2-09-padding/09 2-10-cbc/10 2-11-ecb_detect/11 2-12-ecb_decrypt/12 2-13/13 2-14-ecb_decrypt/14 2-15-padding/15 2-16/16 3-21-mersenne_twister/mersenne_twister 3-22/mersenne_twister

.PHONY: clean

all: $(bins)

clean:
	-rm -f $(bins) $(libdir)/*.{[oa],so}

$(libdir)/libkrypto.o: $(libdir)/libkrypto.c $(libdir)/libkrypto.h
	gcc -c -fpic $(@D)/libkrypto.c -o $@

$(libdir)/hacking.o: $(libdir)/hacking.c $(libdir)/hacking.h
	gcc -c -fpic $(@D)/hacking.c -o $@ -Wall

$(libdir)/libkrypto.a: $(libdir)/libkrypto.o $(libdir)/hacking.o
	ar rs $(@D)/libkrypto.a $(@D)/libkrypto.o $(@D)/hacking.o

$(libdir)/libkrypto.so: $(libdir)/libkrypto.o $(libdir)/hacking.o
	gcc -shared -o $@ $(@D)/libkrypto.o $(@D)/hacking.o -lcrypto -Wall

1-03/03: 1-03/03.c
	gcc $< -o $@

1-04/04: 1-04/04.c
	gcc $< -o $@ -Wall

1-04/04.txt:
	wget -nv -O $@ https://gist.githubusercontent.com/tqbf/3132713/raw/40da378d42026a0731ee1cd0b2bd50f66aabac5b/gistfile1.txt

1-05/vigenere: 1-05/vigenere.c
	gcc $< -o $@

1-05/05: 1-05/05.c
	gcc $< -o $@ -Wall

1-06-hamming_distance/06: 1-06-hamming_distance/06.c $(libdir)/libkrypto.a
	gcc --static -L$(libdir) $< -o $@ -lkrypto -lcrypto

1-06-hamming_distance/06.txt:
	wget -nv -O $@ https://gist.githubusercontent.com/tqbf/3132752/raw/cecdb818e3ee4f5dda6f0847bfd90a83edb87e73/gistfile1.txt

1-07/07: 1-07/07.c $(libdir)/libkrypto.a
	gcc --static -L$(libdir) -o $@ $< -lkrypto -lcrypto

1-07/07.txt:
	wget -nv -O $@ https://gist.githubusercontent.com/tqbf/3132853/raw/c02ff8a08ccf872f4cd278396379f4bb1ef337d8/gistfile1.txt

1-07/encrypt: 1-07/encrypt.c $(libdir)/libkrypto.a
	gcc --static -L$(libdir) -o $@ $< -lkrypto -lcrypto

1-08/08.txt:
	wget -nv -O $@ https://gist.githubusercontent.com/tqbf/3132928/raw/6f74d4131d02dee3dd0766bd99a6b46c965491cc/gistfile1.txt

2-09-padding/09: 2-09-padding/09.c $(libdir)/libkrypto.a
	gcc --static -L$(libdir) -o $@ $< -lkrypto -lcrypto

2-10-cbc/10: 2-10-cbc/10.c $(libdir)/libkrypto.a
	gcc --static -L$(libdir) -o $@ $< -lkrypto -lcrypto

2-10-cbc/10.txt:
	wget -nv -O $@ https://gist.githubusercontent.com/tqbf/3132976/raw/f0802a5bc9ffa2a69cd92c981438399d4ce1b8e4/gistfile1.txt

2-11-ecb_detect/11: 2-11-ecb_detect/11.c $(libdir)/libkrypto.a
	gcc --static -L$(libdir) -o $@ $< -lkrypto -lcrypto

2-12-ecb_decrypt/12: 2-12-ecb_decrypt/12.c $(libdir)/libkrypto.a
	gcc --static -L$(libdir) -o $@ $< -lkrypto -lcrypto

2-13/13: 2-13/13.c $(libdir)/libkrypto.so
	gcc -L$(libdir) -o $@ $< -lkrypto -lcrypto -Wl,-rpath='$$ORIGIN/../$(libdir)'

2-14-ecb_decrypt/14: 2-14-ecb_decrypt/14.c $(libdir)/libkrypto.a
	gcc --static -L$(libdir) -o $@ $< -lkrypto -lcrypto

2-14-ecb_decrypt/12.txt: 2-12-ecb_decrypt/12.txt
	ln -sv ../2-12-ecb_decrypt/$(@F) $@

2-15-padding/15: 2-15-padding/15.c $(libdir)/libkrypto.a
	gcc --static -L$(libdir) -o $@ $< -lkrypto -lcrypto -Wall

2-16/16: 2-16/16.c $(libdir)/libkrypto.so
	gcc -L$(libdir) -o $@ $< -lkrypto -lcrypto -Wl,-rpath='$$ORIGIN/../$(libdir)'

3-21-mersenne_twister/mersenne_twister: 3-21-mersenne_twister/mersenne_twister.c $(libdir)/libkrypto.so
	gcc -L$(libdir) -o $@ $< -lkrypto -lcrypto -Wl,-rpath='$$ORIGIN/../$(libdir)' -Wall

3-22/mersenne_twister: 3-22/mersenne_twister.c $(libdir)/libkrypto.so
	gcc -L$(libdir) -o $@ $< -lkrypto -lcrypto -Wl,-rpath='$$ORIGIN/../$(libdir)' -Wall
