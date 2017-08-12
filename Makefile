SUBDIRS = 1-03 1-04 1-05 1-06-hamming_distance lib 1-07 2-09-padding 2-10-cbc 2-11-ecb_detect 2-12-ecb_decrypt 2-13 2-14-ecb_decrypt 2-15-padding 2-16 3-21-mersenne_twister 3-22

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
