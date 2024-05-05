all: play

play: render
	./render ./video.h264

render: render.c video.h264
	cc -O3 render.c -o render -lavcodec -lglfw -framework opengl

video.h264: encode
	./encode

encode: encode.c
	cc -O3 encode.c -o encode -lx264

clean:
	rm -f encode render
