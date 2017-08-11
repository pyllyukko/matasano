default: static

static: ../lib/libkrypto.a
	gcc --static -g -L../lib -L/usr/lib -o 13 13.c -lkrypto -lcrypto

dynamic: ../lib/libkrypto.so
	gcc -I. -L../lib -o 13 13.c -lkrypto -lcrypto
