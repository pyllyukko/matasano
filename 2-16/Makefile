.PHONY: static dynamic

static: ../lib/libkrypto.a
	gcc --static -g -L../lib -L/usr/lib -o 16 16.c -lkrypto -lcrypto

dynamic: ../lib/libkrypto.so
	gcc -I. -L../lib -o 16 16.c -lkrypto -lcrypto -Wl,-rpath='$$ORIGIN/../lib'
