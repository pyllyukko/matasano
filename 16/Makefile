default: static

static:
	gcc --static -g -L../lib -L/usr/lib -o 16 16.c -lkrypto -lcrypto

dynamic:
	gcc -I. -L../lib -o 16 16.c -lkrypto -lcrypto
