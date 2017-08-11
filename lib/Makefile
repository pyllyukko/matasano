default: static
dynamic:
	gcc -c -fpic libkrypto.c hacking.c
	gcc -shared -o libkrypto.so libkrypto.o hacking.o -lcrypto
static:
	gcc -c libkrypto.c hacking.c
	ar rs libkrypto.a libkrypto.o hacking.o
clean:
	rm -fv libkrypto.a
	rm -fv libkrypto.so
