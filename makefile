SRC_DIR=$(shell pwd)/

all: rebuild

rebuild:
	mkdir -p src/obj
	$(MAKE) -C src ../a.out

.PHONY: all clean

clean:
	$(MAKE) -C src clean
