SUBDIRS = 03 04 05 06-hamming_distance lib 07 09-padding 10-cbc 11-ecb_detect 12-ecb_decrypt 13 14-ecb_decrypt 15-padding 16 21-mersenne_twister 22

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

06-hamming_distance: lib

07: lib

09-padding: lib

10-cbc: lib

11-ecb_detect: lib

12-ecb_decrypt: lib

13: lib

14-ecb_decrypt: lib

15-padding: lib

16: lib

21-mersenne_twister: lib

22: lib
