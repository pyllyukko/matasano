.PHONY: clean

libkrypto.a:
	gcc -c libkrypto.c hacking.c
	ar rs libkrypto.a libkrypto.o hacking.o

libkrypto.so:
	gcc -c -fpic libkrypto.c hacking.c
	gcc -shared -o libkrypto.so libkrypto.o hacking.o -lcrypto

clean:
	rm -fv *.o
	rm -fv libkrypto.a
	rm -fv libkrypto.so
