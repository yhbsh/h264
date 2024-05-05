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
	cc -O3 $(INCLUDES) encode.c -o encode $(LIBS) $(FRAMEWORKS)

clean:
	rm -f encode render
