# please install libdrm development dependency
# this program has been tested on linux/ubuntu/i915 machine

cflags=`pkg-config --cflags --libs libdrm`

all:
	gcc -o drm font_8x8.c font_8x16.c drm.c ${cflags}

clean:
	rm drm
