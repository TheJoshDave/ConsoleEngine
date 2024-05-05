#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <windows.h>
#include "WindowsConsoleRenderer.h"
float deltaTime = 1;
float scale = pow(((1.0/6)*(1.0/12)*(1/0.3048)),-1.0);
float jump = 2;
float gravity = 9.8;
int RESX, RESY;
int temp;

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    float max_vx;
    float max_vy;
    float min_vx;
    float min_vy;
    float ax;
    float ay;
    console_pixel *image;
    int w;
    int h;
} object;

LARGE_INTEGER update_deltaTime(LARGE_INTEGER lastTicks) {
    LARGE_INTEGER frequency;
    LARGE_INTEGER newTicks;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&newTicks);
    if (lastTicks.QuadPart)
        deltaTime = (float)(newTicks.QuadPart - lastTicks.QuadPart) / frequency.QuadPart;
    return newTicks;
}

double clamp(double d, double min, double max) {
  const double t = d < min ? min : d;
  return t > max ? max : t;
}
// last four bits hold if object ran into wall: v<^> = 1010
int physics_object(object *object, int max_x, int max_y, int clamp_pos) {
    //int min_x = -object->w, min_y = -object->h; // good for hiding at the edges
    max_x-=object->w; max_y-=object->h-1; int min_x = 1, min_y = 1; // clamp to edge

    float x = object->x += object->vx * deltaTime * scale;
    float y = object->y += object->vy * deltaTime * scale;
    if (clamp_pos) {
        object->x = clamp(x, min_x, max_x);
        object->y = clamp(y, min_y, max_y);
    }
    float vx = object->vx += object->ax * deltaTime; // should be half before and after but whatever
    float vy = object->vy += object->ay * deltaTime;
    object->vx = clamp(vx, object->min_vx, object->max_vx);
    object->vy = clamp(vy, object->min_vy, object->max_vy);
    int walls = (max_y < y)<<3 | (x < min_x)<<2 | (y < min_y)<<1 | (max_x < x)<<0;
    return walls;
}
// called stack_ pshysics as pun of stack_sreens because this only works in the physics engine meaning i should probably actually implement this work out of this code and into the renderer code
void stack_physics(object base, object top){
    stack_console_screens(base.image, base.w, base.h,
                          top.image, top.w, top.h, top.y, top.x);
}
// creates square object with one repeating pixel
// input: object, height, width, x_pos, y_pos, image_character, image_colorfg, image_colorbg;
void initialize_object(object *o, console_pixel *i, int h, int w, float x, float y, char c, int f, int b) {
    o->h=h; o->w=w; o->x=x; o->y=y;
    o->vx=0; o->ax=0; o->vy=0; o->ay=0;
    o->min_vx=-10; o->max_vx=10; o->min_vy=-10; o->max_vy=10;
    fill_console_pixel_array(i, w, h, (console_pixel){c, f, b});
    o->image = i;
}
// user input live
int user_input() {
    if (kbhit()) {
        int current_key_code = getch();
        fflush(stdin);
        return current_key_code;
    }
    return 0;
}

int main() {
    printf("Enter any number to start: "); scanf("%d", &temp);
    screen_size(&RESX,&RESY); // if this didnt need to happen here then the prinf could go away, how to do this: by updating whenever the window size changes which we can probably detect with some variable

    // initialize objects and images
    object background;  console_pixel background_image[RESY][RESX];
    initialize_object(&background, &background_image, RESY, RESX, 1, 1, ' ', 0x4EBAC400, 0x00000000);

    //object tube;  console_pixel tube_image[RESY][RESX]; //initialize
    //initialize_object(&tube, &tube_image, 5, 2, 1, 20, ' ', 0xA3F45000, 0x00000000);
    // 1+RESY/4


    object pipe;  console_pixel pipe_image[RESY][RESX]; //initialize
    object pipe1;  console_pixel pipe_image1[RESY][RESX]; //initialize
    object pipe2;  console_pixel pipe_image2[RESY][RESX]; //initialize
    object pipe3;  console_pixel pipe_image3[RESY][RESX]; //initialize
    object pipe4;  console_pixel pipe_image4[RESY][RESX]; //initialize
    object pipe5;  console_pixel pipe_image5[RESY][RESX]; //initialize
    initialize_object(&pipe, &pipe_image, RESY, 2, RESX, 1, ' ', 0xA3F45000, 0x00000000);
    initialize_object(&pipe1, &pipe_image1, 10, 2, (RESX*1/6.0), 1, ' ', 0xA3F45000, 0x00000000);
    initialize_object(&pipe2, &pipe_image2, 10, 2, (RESX*2/6.0), 1, ' ', 0xA3F45000, 0x00000000);
    initialize_object(&pipe3, &pipe_image3, 10, 2, (RESX*3/6.0), 1, ' ', 0xA3F45000, 0x00000000);
    initialize_object(&pipe4, &pipe_image4, 10, 2, (RESX*4/6.0), 1, ' ', 0xA3F45000, 0x00000000);
    initialize_object(&pipe5, &pipe_image5, 10, 2, (RESX*5/6.0), 1, ' ', 0xA3F45000, 0x00000000);
    pipe.vx=-1;
    pipe1.vx=-1;
    pipe2.vx=-1;
    pipe3.vx=-1;
    pipe4.vx=-1;
    pipe5.vx=-1;

    object player;  console_pixel player_image[RESY][RESX];
    initialize_object(&player, &player_image, RESY/8*12/17.0, RESY/8, RESY/3.0, 1, '>', 0xF9DC3500, 0xFE980000);
    player.ay=gravity;

    object screen;  console_pixel screen_image[RESY][RESX];
    initialize_object(&screen, &screen_image, RESY, RESX, 1, 1, ' ', 0x00000000, 0x00000000);

    LARGE_INTEGER lastTicks;lastTicks.QuadPart = 0;
    while (1) {;
        Sleep(1000/30);
        lastTicks = update_deltaTime(lastTicks);

        physics_object(&player, RESX, RESY, 1);
        if (physics_object(&pipe, RESX, RESY, 0)==0b0100) pipe.x+=RESX;
        if (physics_object(&pipe1, RESX, RESY, 0)==0b0100) pipe1.x+=RESX;
        if (physics_object(&pipe2, RESX, RESY, 0)==0b0100) pipe2.x+=RESX;
        if (physics_object(&pipe3, RESX, RESY, 0)==0b0100) pipe3.x+=RESX;
        if (physics_object(&pipe4, RESX, RESY, 0)==0b0100) pipe4.x+=RESX;
        if (physics_object(&pipe5, RESX, RESY, 0)==0b0100) pipe5.x+=RESX;
        if(user_input()) player.vy = -jump-(player.ay*deltaTime/2);

        stack_physics(screen, background);
        stack_physics(screen, pipe);
        stack_physics(screen, pipe1);
        stack_physics(screen, pipe2);
        stack_physics(screen, pipe3);
        stack_physics(screen, pipe4);
        stack_physics(screen, pipe5);
        stack_physics(screen, player);
        print_full_console(screen.image, RESX, RESY);
    }
    return 0;
}
