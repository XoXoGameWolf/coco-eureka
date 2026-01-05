all: build/app
	@build/app

build/app: src/main.c
	@gcc -o build/app src/main.c `sdl2-config --cflags --libs`