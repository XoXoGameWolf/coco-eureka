#include "coco_eureka.c"

char* title = "Coco Eureka Engine";
IVec2 res = {1920, 1080};
Color bg = {32, 64, 128};

Object* icon;
Sound space;

void start() {
    icon = createObject((IVec2){704, 284}, (IVec2){512, 512}, loadImage("resources/icon.png"));
    playMusic(createMusic("resources/music.wav"), true);
    space = createSound("resources/space.wav");
    Image text = renderFont(createFont("resources/FreeSerif.ttf", 32), "Coco Eureka Engine v0.1.0", (Color){255, 255, 255});
    createObject((IVec2){0, 0}, (IVec2){text.surf->w, text.surf->h}, text);
}

void update() {
    if(getKeyState(KEY_W)) icon->pos.y += 3;
    if(getKeyState(KEY_A)) icon->pos.x += 3;
    if(getKeyState(KEY_S)) icon->pos.y -= 3;
    if(getKeyState(KEY_D)) icon->pos.x -= 3;

    if(getKeyDown(KEY_SPACE)) playSound(space);
}