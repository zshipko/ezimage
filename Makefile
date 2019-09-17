dest?=/usr/local
build?=./build

# Default to `rust` backend if cargo is available
ifeq (, $(shell which cargo))
	backend?=stb_image
else
	backend?=rust
endif

.PHONY: build
build:
	mkdir -p $(build)/lib $(build)/include
	cd src/$(backend) && $(MAKE)
	cp src/$(backend)/libezimageio_impl.a $(build)/lib/libezimageio.a
	cp src/ezimageio.h $(build)/include/libezimageio.h
	cp src/$(backend)/ezimageio.pc ./ezimageio.pc

install:
	mkdir -p $(dest)/include $(dest)/lib/pkgconfig
	install $(build)/lib/libezimageio.a $(dest)/lib
	install $(build)/include/ezimageio.h $(dest)/include
	@cat ezimageio.pc | sed -e 's|@dest|'"$(dest)"'|g' > $(dest)/lib/pkgconfig/ezimageio.pc

uninstall:
	rm -f $(dest)/lib/libezimageio.a \
		  $(dest)/include/ezimageio.h \
		  $(dest)/lib/pkgconfig/ezimageio.pc

test/big.png:
	curl https://eoimages.gsfc.nasa.gov/images/imagerecords/74000/74393/world.topo.200407.3x5400x2700.png > test/big.png

.PHONY: test
test: test/big.png build
	@cat ezimageio.pc | sed -e 's|@dest|build|g' > ezimageio-test.pc
	@PKG_CONFIG_LIBDIR="." $(CC) -o test/test test/test.c -L. `pkg-config --cflags --libs ezimageio-test`
	@time test/test test/big.png

clean:
	cd src/$(backend) && $(MAKE) clean
	rm -rf $(build)/lib/libezimageio.a $(build)/include/ezimage.h ./*.pc out.jpg
