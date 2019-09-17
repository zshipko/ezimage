build:
	$(CC) -c -o ezimageio_impl.o ezimageio.c
	$(AR) rcs libezimageio_impl.a ezimageio_impl.o

update:
	curl -O https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
	curl -O https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h

clean:
	rm -f ezimageio_impl.o libezimageio_impl.a
