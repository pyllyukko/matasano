default: static
dynamic:
	gcc -c -fpic libkrypto.c
	gcc -shared -o libkrypto.so libkrypto.o -lcrypto
static:
	gcc -c libkrypto.c
	ar rs libkrypto.a libkrypto.o
clean:
	rm -fv libkrypto.a
	rm -fv libkrypto.so
