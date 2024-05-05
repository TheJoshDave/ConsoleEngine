#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <windows.h>
#include "WindowsConsoleRenderer.h"
int RESX, RESY;

int main() {
    screen_size(&RESX,&RESY);

    console_pixel background[RESY][RESX]; //initialize
    fill_console_pixel_array(background, RESX, RESY, (console_pixel){' ', 0x4EBAC400, 0x00000000});

    console_pixel pipe[10][2]; //initialize
    fill_console_pixel_array(pipe, 2, 10, (console_pixel){' ', 0xA3F45000, 0x00000000});

    int pipe_posx = 0;
    int pipe_posy = 0; //rand()%40;
    while (1) {
        console_pixel complete[RESY][RESX]; //initialize
        stack_console_screens(complete, RESX, RESY, background, RESX, RESY, 1, 1);
        stack_console_screens(complete, RESX, RESY, pipe, 2, 10, 1, pipe_posx);
        print_full_console(complete, RESX, RESY);
        pipe_posx++;
        Sleep(1000/60);
    }
    return 0;
}
