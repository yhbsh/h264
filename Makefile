CFLAGS   := $(shell pkg-config --cflags libavcodec glfw3 openh264 x264)
LDFLAGS  := $(shell pkg-config --libs   libavcodec glfw3 openh264 x264) -framework OpenGL

all: encode render decode

render: render.c
	cc $(CFLAGS) render.c -o render $(LDFLAGS)

encode: encode.c
	cc $(CFLAGS) encode.c -o encode $(LDFLAGS)

decode: decode.cpp
	c++ $(CFLAGS) decode.cpp -o decode $(LDFLAGS)

clean:
	rm -f encode render decode
