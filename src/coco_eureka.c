#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "keys.c"

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

typedef struct {
    int x;
    int y;
} IVec2;

typedef struct {
    int x;
    int y;
    int z;
} IVec3;

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    SDL_Texture* tex;
    SDL_Surface* surf;
} Image;

typedef struct {
    IVec2 pos;
    IVec2 scale;
    Image image;
} Object;

typedef struct {
    Mix_Music* music;
} Music;

typedef struct {
    Mix_Chunk* chunk;
} Sound;

typedef struct {
    TTF_Font* font;
} Font;

Image* images;
Object** objects;
Music* musics;
Sound* sounds;
Font* fonts;

bool* keys;
bool* lastKeys;
bool* buttons;
bool* lastButtons;

int mouse_x;
int mouse_y;
int last_mouse_x;
int last_mouse_y;
int scroll_x;
int scroll_y;

SDL_Window* window;
SDL_Renderer* renderer;
bool running = true;
bool fullscreen = false;
int fps = 0;

char* title;
IVec2 res;
Color bg;

void start();
void update();

Font createFont(char* path, int size) {
    TTF_Font* _font = TTF_OpenFont(path, size);
    Font font = {_font};

    for(int i = 0; i < 256; i++) {
        if(fonts[i].font == NULL) {
            fonts[i] = font;
            break;
        }
    }

    return font;
}

Image renderFont(Font font, char* text, Color color) {
    SDL_Color _color = {color.r, color.g, color.b};

    SDL_Surface* surf = TTF_RenderText_Solid(font.font, text, _color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    Image image = {tex, surf};

    for(int i = 0; i < 256; i++) {
        if(images[i].tex == NULL) {
            images[i] = image;
            break;
        }
    }

    return image;
}

void destroyFont(Font font) {
    for(int i = 0; i < 256; i++) {
        if(fonts[i].font == font.font) {
            fonts[i].font = NULL;
            break;
        }
    }

    TTF_CloseFont(font.font);
}

Music createMusic(char* path) {
    Mix_Music* _music = Mix_LoadMUS(path);
    Music music = {_music};

    for(int i = 0; i < 256; i++) {
        if(musics[i].music == NULL) {
            musics[i] = music;
            break;
        }
    }

    return music;
}

void playMusic(Music music, bool loop) {
    Mix_PlayMusic(music.music, loop ? -1 : 0);
}

void pauseMusic() {
    Mix_PauseMusic();
}

void resumeMusic() {
    Mix_ResumeMusic();
}

void stopMusic() {
    Mix_HaltMusic();
}

void destroyMusic(Music music) {
    for(int i = 0; i < 256; i++) {
        if(musics[i].music == music.music) {
            musics[i].music = NULL;
            break;
        }
    }

    Mix_FreeMusic(music.music);
}

Sound createSound(char* path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path);
    Sound sound = {chunk};

    for(int i = 0; i < 256; i++) {
        if(sounds[i].chunk == NULL) {
            sounds[i] = sound;
            break;
        }
    }

    return sound;
}

void playSound(Sound sound) {
    Mix_PlayChannel(-1, sound.chunk, 0);
}

void destroySound(Sound sound) {
    for(int i = 0; i < 256; i++) {
        if(sounds[i].chunk == sound.chunk) {
            sounds[i].chunk = NULL;
            break;
        }
    }

    Mix_FreeChunk(sound.chunk);
}

Object* createObject(IVec2 pos, IVec2 scale, Image image) {
    Object* object = malloc(sizeof(Object));
    object->pos = pos;
    object->scale = scale;
    object->image = image;

    for(int i = 0; i < 256; i++) {
        if(objects[i] == NULL) {
            objects[i] = object;
            break;
        }
    }

    return object;
}

void destroyObject(Object* object) {
    for(int i = 0; i < 256; i++) {
        if(objects[i] == object) {
            objects[i] = NULL;
            break;
        }
    }

    free(object);
}

Image loadImage(char* path) {
    SDL_Surface* _image = IMG_Load(path);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, _image);
    Image image = {tex, _image};

    for(int i = 0; i < 256; i++) {
        if(images[i].tex == NULL) {
            images[i] = image;
            break;
        }
    }

    return image;
}

void saveBitmap(Image image, char* path) {
    SDL_SaveBMP(image.surf, path);
}

void destroyImage(Image image) {
    SDL_DestroyTexture(image.tex);
    SDL_FreeSurface(image.surf);

    for(int i = 0; i < 256; i++) {
        if(images[i].tex == image.tex) {
            images[i].tex = NULL;
            images[i].surf = NULL;
            break;
        }
    }
}

bool getKeyState(SDL_Scancode key) {
    return keys[key];
}

bool getKeyDown(SDL_Scancode key) {
    return !lastKeys[key] && keys[key];
}

bool getKeyUp(SDL_Scancode key) {
    return lastKeys[key] && !keys[key];
}

int getTicks() {
    return SDL_GetTicks();
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    TTF_Init();

    images = calloc(sizeof(Image), 256);
    objects = calloc(sizeof(Object*), 256);
    musics = calloc(sizeof(Music), 256);
    sounds = calloc(sizeof(Sound), 256);
    fonts = calloc(sizeof(Font), 256);
    keys = calloc(sizeof(bool), SDL_NUM_SCANCODES);
    lastKeys = calloc(sizeof(bool), SDL_NUM_SCANCODES);
    buttons = calloc(sizeof(bool), 6);
    lastButtons = calloc(sizeof(bool), 6);
    mouse_x = 0;
    mouse_y = 0;
    last_mouse_x = 0;
    last_mouse_y = 0;
    scroll_x = 0;
    scroll_y = 0;

    window = SDL_CreateWindow(title, 0, 0, res.x, res.y, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderSetLogicalSize(renderer, 1920, 1080);

    start();

    SDL_Event event;

    while(running) {
        int fps_start = getTicks();
        while(SDL_PollEvent(&event) != 0) {
            switch(event.type) {
                case(SDL_QUIT): running = false; break;
                case(SDL_KEYDOWN): keys[event.key.keysym.scancode] = true; break;
                case(SDL_KEYUP): keys[event.key.keysym.scancode] = false; break;
                case(SDL_MOUSEBUTTONDOWN): buttons[event.button.button] = true; break;
                case(SDL_MOUSEBUTTONUP): buttons[event.button.button] = false; break;
                case(SDL_MOUSEMOTION): mouse_x = (int)event.motion.x; mouse_y = (int)event.motion.y; break;
                case(SDL_MOUSEWHEEL): scroll_x = (int)event.wheel.x; scroll_y = (int)event.wheel.y; break;
            }
        }

        if(getKeyDown(KEY_F11)) {
            if(fullscreen) {
                SDL_SetWindowFullscreen(window, 0);
                fullscreen = false;

            } else {
                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                fullscreen = true;
            }
        }

        update();

        SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, 255);
        SDL_RenderClear(renderer);

        for(int i = 0; i < 256; i++) {
            if(objects[i] != NULL) {
                SDL_Rect dest = {
                    objects[i]->pos.x,
                    objects[i]->pos.y,
                    objects[i]->scale.x,
                    objects[i]->scale.y
                };

                SDL_RenderCopy(renderer, objects[i]->image.tex, NULL, &dest);
            }
        }

        SDL_RenderPresent(renderer);

        memcpy(lastKeys, keys, SDL_NUM_SCANCODES);
        memcpy(lastButtons, buttons, 6);
        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;
        scroll_x = 0;
        scroll_y = 0;

        int fps_end = getTicks();
        fps = 1.0 / ((float)(fps_end - fps_start) / 1000.0);
    }

    for(int i = 0; i < 256; i++) {
        if(images[i].tex != NULL) {
            SDL_DestroyTexture(images[i].tex);
            SDL_FreeSurface(images[i].surf);
        }

        if(musics[i].music != NULL) {
            Mix_FreeMusic(musics[i].music);
        }

        if(sounds[i].chunk != NULL) {
            Mix_FreeChunk(sounds[i].chunk);
        }

        if(fonts[i].font != NULL) {
            TTF_CloseFont(fonts[i].font);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    free(images);
    free(objects);
    free(musics);
    free(sounds);
    free(fonts);
    free(keys);
    free(lastKeys);
    free(buttons);
    free(lastButtons);

    return 0;
}