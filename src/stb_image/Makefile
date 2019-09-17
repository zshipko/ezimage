build:
	$(CC) -c -o ezimage_impl.o ezimage.c
	$(AR) rcs libezimage_impl.a ezimage_impl.o

update:
	curl -O https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
	curl -O https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h

clean:
	rm -f ezimage_impl.o libezimage_impl.a
