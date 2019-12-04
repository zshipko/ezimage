dest?=/usr/local
build?=./build
backend?=default
PIC?=-fPIC
CXXLIB?=-lstdc++
UNAME_S:=$(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	CXXLIB=-lc++
	LIBC++=yes
endif

.PHONY: build
build:
	mkdir -p $(build)/lib $(build)/include
	cd src/$(backend) && $(MAKE) CFLAGS=$(PIC)
	cp src/$(backend)/libezimage_impl.a $(build)/lib/libezimage.a
	cp src/ezimage.h $(build)/include/ezimage.h
	cp src/$(backend)/ezimage.pc ./ezimage.pc

install:
	mkdir -p $(dest)/include $(dest)/lib/pkgconfig
	install $(build)/lib/libezimage.a $(dest)/lib
	install $(build)/include/ezimage.h $(dest)/include
	@cat ezimage.pc | sed -e 's|@dest|'"$(dest)"'|g' -e 's|@CXX|'"$(CXXLIB)"'|g' > $(dest)/lib/pkgconfig/ezimage.pc

uninstall:
	rm -f $(dest)/lib/libezimage.a \
		  $(dest)/include/ezimage.h \
		  $(dest)/lib/pkgconfig/ezimage.pc

test/big.png:
	curl https://eoimages.gsfc.nasa.gov/images/imagerecords/74000/74393/world.topo.200407.3x5400x2700.png > test/big.png

.PHONY: test
test: test/big.png build
	cat ezimage.pc | sed -e 's|@dest|./build|g' -e 's|@CXX|'"$(CXXLIB)"'|g' > ezimage-test.pc
	$(CC) -o test/test test/test.c -Lbuild/lib `PKG_CONFIG_PATH="${PWD}" pkg-config --cflags --libs ezimage-test`
	@LD_LIBRARY_PATH=./build/lib test/test test/big.png && echo

test-all:
	$(MAKE) backend=default test
	$(MAKE) backend=rust test
	$(MAKE) backend=openimageio test

clean:
	cd src/$(backend) && $(MAKE) clean
	rm -rf $(build)/lib/libezimage.a $(build)/include/ezimage.h ./*.pc out.jpg
