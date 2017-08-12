bins=encrypt 07
.PHONY: all clean
all: $(bins)
encrypt: ../lib/libkrypto.a
	gcc --static -L../lib -L/usr/lib -o encrypt encrypt.c -lkrypto -lcrypto
07: ../lib/libkrypto.a
	gcc --static -L../lib -L/usr/lib -o 07 07.c -lkrypto -lcrypto
clean:
	rm -f $(bins)
