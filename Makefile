LIST = 03 04
all:
	for i in $(LIST); do \
		gcc $$i/$$i.c -o $$i/$$i; \
	done
	+$(MAKE) -C 05
	+$(MAKE) -C lib
	+$(MAKE) -C 07
	+$(MAKE) -C 09-padding
	+$(MAKE) -C 10-cbc
	+$(MAKE) -C 11-ecb_detect
	+$(MAKE) -C 12-ecb_decrypt
	+$(MAKE) -C 13
	+$(MAKE) -C 14-ecb_decrypt
	+$(MAKE) -C 15-padding
	+$(MAKE) -C 16
	+$(MAKE) -C 21-mersenne_twister
	+$(MAKE) -C 22
