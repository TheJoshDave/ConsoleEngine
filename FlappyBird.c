#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <windows.h>
float deltaTime = 1;
float scale = pow(((1.0/6)*(1.0/12)*(1/0.3048)),-1.0);
float jump = 2 * 21.9456;
float gravity = 9.8 * 21.9456;
int RESX, RESY;

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    float min_vx;
    float min_vy;
    float max_vx;
    float max_vy;
    float ax;
    float ay;
} physics_profile;
typedef struct { //console_screen
    int row;
    int column;
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
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 0;
} // updates the input variables to the current screen size
console_screen initialize_console_screen() {
    console_screen screen;
    screen.row = 1; screen.column = 1;
    screen_size(&screen.width, &screen.height);
    int array_size = screen.width * screen.height * sizeof(int);
    screen.colorsbg = malloc(array_size);
    screen.colorsfg = malloc(array_size);
    screen.characters = malloc(array_size);
    return screen;
}
console_screen initialize_console_object(console_screen *screen) {
    int array_size = screen->width * screen->height * sizeof(int);
    screen->colorsbg = malloc(array_size);
    screen->colorsfg = malloc(array_size);
    screen->characters = malloc(array_size);
}
void update_screen(console_screen *screen) {
    screen_size(&screen->width, &screen->height);

    int array_size = screen->width * screen->height * sizeof(int);
    free(screen->colorsbg); screen->colorsbg = malloc(array_size);
    free(screen->colorsfg); screen->colorsfg = malloc(array_size);
    free(screen->characters); screen->characters = malloc(array_size);

    //realloc(screen->colorsbg, array_size);
    //realloc(screen->colorsfg, array_size);
    //realloc(screen->characters, array_size);
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
    system("cls");
    puts(buffer);
}
void fill_console_screen(console_screen *screen, char character, int colorbg, int colorfg) {
    for (int i = 0; i < screen->width * screen->height; i++) {
        screen->characters[i] = character;
        screen->colorsbg[i] = colorbg;
        screen->colorsfg[i] = colorfg;
    }
}
void stack_console_screens(console_screen *bottom, console_screen top) {
    int matches = 0;
    for (int y=0; y<bottom->height; y++) {
        if ((top.row<=y+1) && (y+2<=top.row+top.height)) {
            for (int x=0; x<bottom->width; x++) {
                if ((top.column<=x+1) && (x+2<=top.column+top.width)) {
                    int b = x+y*bottom->width;
                    bottom->characters[b] = top.characters[matches];
                    bottom->colorsbg[b] = top.colorsbg[matches];
                    bottom->colorsfg[b] = top.colorsfg[matches];
                    matches++;
                }
            }
        }
    }
}





typedef struct {
    unsigned int background_color; //rgba console color
    unsigned int foreground_color; //rgba text color
    char character;
} static_pixel;
struct object {
    float x;
    float y;
    float vx;
    float vy;
    float ax;
    float ay;
    static_pixel falling;
    static_pixel jumping;
};

LARGE_INTEGER update_deltaTime(LARGE_INTEGER lastTicks) {
    LARGE_INTEGER frequency;
    LARGE_INTEGER newTicks;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&newTicks);
    if (lastTicks.QuadPart)
        deltaTime = (float)(newTicks.QuadPart - lastTicks.QuadPart) / frequency.QuadPart;
    return newTicks;
}
unsigned int color(unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
    return r<<24 | g<<16 | b<<8 | a;
}

void render_pixel(int x, int y, static_pixel pixel) {
    printf("\033[%d;%dH", y, x); // cursor position
    printf("\033[48;2;%d;%d;%dm\033[38;2;%d;%d;%dm%c",
           (pixel.background_color>>24)&0xFF,
           (pixel.background_color>>16)&0xFF,
           (pixel.background_color>>8)&0xFF,
           (pixel.foreground_color>>24)&0xFF,
           (pixel.foreground_color>>16)&0xFF,
           (pixel.foreground_color>>8)&0xFF,
           pixel.character); // color and character
    printf("\033[48;2;0;0;0m\033[38;2;255;255;255m"); // reset formating could be removed
}
void render_object(struct object object) {
    static_pixel pixel = (object.vy>=0 ? object.falling : object.jumping);
    render_pixel((int)round(object.x), (int)round(object.y), pixel);
}

void physics_object(struct object *object) {
    int max_x = 50, max_y = 40;
    float min_vx = 0*scale, min_vy = -2*scale;
    float max_vx = 0*scale, max_vy = 4*scale;
    float x = object->x + object->vx * deltaTime;
    float y = object->y + object->vy * deltaTime;
    object->x = (x<=1) ? 1 : ((x>=max_x) ? max_x : x);
    object->y = (y<=1) ? 1 : ((y>=max_y) ? max_y : y);
    float vx = object->vx + object->ax * deltaTime;
    float vy = object->vy + object->ay * deltaTime;
    object->vx = (vx<=min_vx) ? min_vx : ((vx>=max_vx) ? max_vx : vx);
    object->vy = (vy<=min_vy) ? min_vy : ((vy>=max_vy) ? max_vy : vy);
}
void apply_physics_profile_to_object(console_screen *object, physics_profile *physics) {
    float x = physics->x + physics->vx * deltaTime;
    float y = physics->y + physics->vy * deltaTime;
    //physics->x = (x<=1) ? 1 : ((x>=max_x) ? max_x : x);
    //physics->y = (y<=1) ? 1 : ((y>=max_y) ? max_y : y);
    float vx = physics->vx + physics->ax * deltaTime;
    float vy = physics->vy + physics->ay * deltaTime;
    //physics->vx = (vx<=min_vx) ? min_vx : ((vx>=max_vx) ? max_vx : vx);
    //physics->vy = (vy<=min_vy) ? min_vy : ((vy>=max_vy) ? max_vy : vy);
}



int main() {
    screen_size(&RESX, &RESY);

    struct object player = {5,5,0,0,0,gravity,
    {color(255,255,0,0),color(0,0,0,0),'>'},
    {color(255,255,0,0),color(0,0,0,0),'^'}};
    LARGE_INTEGER lastTicks;lastTicks.QuadPart = 0;

    console_screen background = initialize_console_screen();
    fill_console_screen(&background, ' ', 0x4EBAC400, 0x00000000);

    console_screen tube = {1,10,2,10};
    initialize_console_object(&tube);
    fill_console_screen(&tube, ' ', 0xA3F45000, 0x00000000);

    physics_profile tube_physics = {1,1,1};





    sleep(1);
    while (1){
        //update_screen(&background);
        lastTicks = update_deltaTime(lastTicks);
        physics_object(&player);
        console_screen complete = background;
        stack_console_screens(&complete, tube);

        Sleep(1000/60);
        print_screen(complete);
        render_object(player);
        //printf("%f %f %f", player.y, player.vy, player.ay);

        if (kbhit()) {
            int current_key_code = getch();
            if (current_key_code)
                player.vy = -jump-(player.ay*deltaTime/2);
            fflush(stdin);
        }
    }
    return 0;
}

/*
⸮123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`
abcdefghijklmnopqrstuvwxyz{|}~ÇüéâäàåçêëèïîìÄÅÉæÆ
ôöòûùÿÖÜ¢£¥₧ƒáíóúñÑªº¿⌐¬½¼¡«»░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├
─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠
⌡÷≈°∙·√ⁿ²■ 
*/

/* wasd
switch (current_key_code) {
  case 's':
    player.y++;
    break;
  case 'w':
    player.y--;
    break;
  case 'd':
    player.x++;
    break;
  case 'a':
    player.x--;
    break;
}
*/
