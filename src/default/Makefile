build:
	$(CXX) -std=c++11 -c -o exr.o exr.cpp
	$(CC) -c -o ezimage_impl.o ezimage.c
	$(AR) rcs libezimage_impl.a ezimage_impl.o exr.o

update:
	curl -O https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
	curl -O https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
	curl -O https://raw.githubusercontent.com/syoyo/tinyexr/master/tinyexr.h

clean:
	rm -f ezimage_impl.o libezimage_impl.a
