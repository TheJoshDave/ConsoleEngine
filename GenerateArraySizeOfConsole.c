#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef struct {
    unsigned int colorbg; //rgba console color
    unsigned int colorfg; //rgba text color
    char character;
} console_pixel;
typedef struct {
    int width;
    int height;
    int *characters;
    int *colors;
} screen;
int* array(int size) {
    return malloc(size * sizeof(int));
} // returns 1D array
void screen_size(int *width, int *height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
} // updates the input variables to the current screen size
screen screen_array() {
    int width, height;
    screen_size(&width, &height);
    screen screen = {width, height, array(width*height), array(width*height)};
    return screen;
} // initializes screen structure
screen update_screen(screen screen) {
    screen_size(&screen.width, &screen.height);
    free(screen.characters);
    screen.characters = array(screen.width*screen.height);
    free(screen.colors);
    screen.colors = array(screen.width*screen.height);
    return screen;
} // updates screen structure


int main(void) {
    screen screen = screen_array();

    for (int i = 0; i < screen.width * screen.height; i++)
        screen.characters[i] = 'q';
    for (int i=0; i<screen.height; i++) {
        for (int j=0; j<screen.width; j++)
            printf("%c", screen.characters[i * screen.width + j]);
        printf("\n");
    }

    screen = update_screen(screen);
    for (int i = 0; i < screen.width * screen.height; i++)
        screen.colors[i] = 0;
    for (int i=0; i<screen.height; i++) {
        for (int j=0; j<screen.width; j++)
            printf("%d", screen.colors[i * screen.width + j]);
        printf("\n");
    }

    return 0;
}
