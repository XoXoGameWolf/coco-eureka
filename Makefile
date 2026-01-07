all: build/app
	@build/app

build/app: src/*
	@gcc -o build/app src/main.c `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_mixer -lSDL2_ttf