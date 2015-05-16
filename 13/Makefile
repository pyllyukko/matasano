default: static

static:
	gcc --static -g -L../lib -L/usr/lib -o 13 13.c -lkrypto -lcrypto

dynamic:
	gcc -I. -L../lib -o 13 13.c -lkrypto -lcrypto
