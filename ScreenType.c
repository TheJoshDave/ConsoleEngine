#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

typedef struct {
    int width;
    int height;
    int* colorsbg; //rgba console color
    int* colorsfg; //rgba text color
    int* characters;
} console_screen;
void screen_size(int *width, int *height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
} // updates the input variables to the current screen size
console_screen initialize_console_screen() {
    console_screen screen;
    screen_size(&screen.width, &screen.height);
    int array_size = screen.width * screen.height * sizeof(int);
    screen.colorsbg = malloc(array_size);
    screen.colorsfg = malloc(array_size);
    screen.characters = malloc(array_size);
    return screen;
}
void update_screen(console_screen *screen) {
    screen_size(&screen->width, &screen->height);
    int array_size = screen->width * screen->height * sizeof(int);
    realloc(screen->colorsbg, array_size);
    realloc(screen->colorsfg, array_size);
    realloc(screen->characters, array_size);
}
void print_screen(console_screen screen) {
    int screen_pixels = screen.width * screen.height;
    char buffer[100000] = "";
    int last_colorbg;
    int last_colorfg;
    for (int i = 0; i < screen_pixels; i++) {
        if (screen.colorsbg[i] != last_colorbg) {
            sprintf(buffer, "%s\033[48;2;%d;%d;%dm", buffer,
            (screen.colorsbg[i]>>24)&0xFF,
            (screen.colorsbg[i]>>16)&0xFF,
            (screen.colorsbg[i]>>8)&0xFF);
            last_colorbg = screen.colorsbg[i];
        }
        if (screen.colorsfg[i] != last_colorfg) {
            sprintf(buffer, "%s\033[38;2;%d;%d;%dm", buffer,
            (screen.colorsfg[i]>>24)&0xFF,
            (screen.colorsfg[i]>>16)&0xFF,
            (screen.colorsfg[i]>>8)&0xFF);
            last_colorfg = screen.colorsfg[i];
        }
        sprintf(buffer, "%s%c", buffer, screen.characters[i]);
    }
    printf("%s", buffer);
}

int main(void) {
    console_screen screen = initialize_console_screen();
    for (int i = 0; i < screen.width * screen.height; i++) {
        screen.characters[i] = ' ';
        screen.colorsbg[i] = 0x4EBAC4;
        screen.colorsfg[i] = 0x000000;
    }
    print_screen(screen);

    return 0;
}
