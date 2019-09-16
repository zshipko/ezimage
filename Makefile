backend?=stb_image
dest?=/usr/local

build:
	cd src/$(backend) && $(MAKE)
	cp src/$(backend)/libezimageio_impl.a ./libezimageio.a
	cp src/$(backend)/ezimageio.pc ./ezimageio.pc

install:
	mkdir -p $(dest)/include $(dest)/lib/pkgconfig
	install libezimageio.a $(dest)/lib
	install ezimageio.pc $(dest)/lib/pkgconfig
	install src/ezimageio.h $(dest)/include

uninstall:
	rm -f $(dest)/lib/libezimageio.a $(dest)/include/ezimageio.h

.PHONY: test
test: build
	$(CC) -o test/test test/test.c -L. -lezimageio -lm -lpthread -ldl
	@time test/test test/big.png

clean:
	cd src/$(backend) && $(MAKE) clean
	rm -f ./libezimageio.a ./ezimageio.pc
