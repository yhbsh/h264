INCLUDES=$(shell pkg-config --cflags libavcodec libavformat x264 glfw3)
LIBS=$(shell pkg-config --libs libavcodec libavformat x264 glfw3)
FRAMEWORKS=-framework opengl

all: play

play: render
	./render ./video.h264

render: render.c video.h264
	cc -O3 $(INCLUDES) render.c -o render $(LIBS) $(FRAMEWORKS)

video.h264: encode
	./encode

encode: encode.c
	cc -std=c17 -I /opt/homebrew/include -O3 encode.c -o encode -L /opt/homebrew/lib -lx264

decode: decode.cpp
	c++ -std=c++17 -I /opt/homebrew/include -O3 decode.cpp -o decode -L /opt/homebrew/lib -lopenh264

clean:
	rm -f encode render decode
