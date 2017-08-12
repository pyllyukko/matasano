SUBDIRS = 1-03 1-04 1-05 1-06-hamming_distance lib 1-07 2-09-padding 2-10-cbc 2-11-ecb_detect 2-12-ecb_decrypt 2-13 2-14-ecb_decrypt 2-15-padding 2-16 3-21-mersenne_twister 3-22

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

1-06-hamming_distance: lib

1-07: lib

2-09-padding: lib

2-10-cbc: lib

2-11-ecb_detect: lib

2-12-ecb_decrypt: lib

2-13: lib

2-14-ecb_decrypt: lib

2-15-padding: lib

2-16: lib

3-21-mersenne_twister: lib

3-22: lib
