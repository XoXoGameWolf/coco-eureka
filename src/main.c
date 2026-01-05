#include <stdio.h>
#include <SDL.h>
#include <unistd.h>

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("Coco Eureka", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Surface *display = SDL_GetWindowSurface(window);

    SDL_FillRect(display, NULL, SDL_MapRGB(display->format, 32, 64, 128));
    SDL_UpdateWindowSurface(window);

    sleep(1);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}