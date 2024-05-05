#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <windows.h>
int RESX, RESY;

typedef struct {
    char character;
    int colorsbg; //rgba console color
    int colorsfg; //rgba text color
} console_pixel;
// prints the console_pixel array
void print_full_console(console_pixel *screen, int columns, int rows) {
    char buffer[100000] = "";
    int last_colorbg, last_colorfg;
    for (int i=0;i<columns*rows;i++) {
            if (screen[i].colorsbg != last_colorbg) {
                sprintf(buffer, "%s\033[48;2;%d;%d;%dm", buffer,
                (screen[i].colorsbg>>24)&0xFF,
                (screen[i].colorsbg>>16)&0xFF,
                (screen[i].colorsbg>>8)&0xFF);
                last_colorbg = screen[i].colorsbg;
            }
            if (screen[i].colorsfg != last_colorfg) {
                sprintf(buffer, "%s\033[38;2;%d;%d;%dm", buffer,
                (screen[i].colorsfg>>24)&0xFF,
                (screen[i].colorsfg>>16)&0xFF,
                (screen[i].colorsfg>>8)&0xFF);
                last_colorfg = screen[i].colorsfg;
            }
            sprintf(buffer, "%s%c", buffer, screen[i].character);
            if (((i+1)%columns==0) && (i<columns*rows-1)) sprintf(buffer, "%s\n", buffer);
            //if ((i+1)%columns==0) printf("potato");
    }
    sprintf(buffer, "%s", buffer);
    system("cls");
    fputs(buffer, stdout);
    //puts(buffer);
}
// puts the size of the console (in characters) into the parameters
void screen_size(int *width, int *height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
} // updates the input variables to the current screen size
// fills a console_pixel array with pixel data
void fill_console_pixel_array(console_pixel *array, int rows, int columns, console_pixel pixel) {
    for (int i=0;i<rows*columns;i++)
        array[i] = pixel;
}
// stacks the second array on top of the first array at the specified location
// replaces first array
void stack_console_screens(console_pixel *bottom, int columns, int rows,
console_pixel *top, int width, int height, int row, int column) {
    int matches = 0;
    for (int y=1; y<=rows; y++) {
        if ((row<=y) && (y<row+height)) {
            for (int x=1; x<=columns; x++) {
                if ((column<=x) && (x<column+width)) {
                    int b = (x-1)+(y-1)*columns;
                    bottom[b].character = top[matches].character;
                    bottom[b].colorsbg = top[matches].colorsbg;
                    bottom[b].colorsfg = top[matches].colorsfg;
                    matches++;
                }
            }
        }
    }


}
// fill part of array with new console pixel
void replace_console_pixel_array(console_pixel *array, int start, int num, console_pixel pixel) {
    for (int i=start;i<num;i++)
        array[i] = pixel;
}
// fill part of array with empty console pixel
void empty_console_pixel_array(console_pixel *array, int start, int num) {
    for (int i=start;i<num+start;i++)
        array[i] = (console_pixel){' ', 0x000000FF, 0x000000FF};
}
