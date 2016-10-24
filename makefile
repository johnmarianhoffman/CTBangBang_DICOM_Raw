SRC_DIR=$(shell pwd)/

all: rebuild

rebuild:
	mkdir -p src/obj
	$(MAKE) -C src ../raw_test
	tests/basic.sh

.PHONY: all clean

clean:
	$(MAKE) -C src clean
