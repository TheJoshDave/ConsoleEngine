#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <windows.h>
float deltaTime = 1;
float scale = pow(((1.0/6)*(1.0/12)*(1/0.3048)),-1.0);
float jump = 2 * 21.9456;
float gravity = 9.8 * 21.9456;

struct object {
    float x;
    float y;
    float vx;
    float vy;
    float ax;
    float ay;
    char falling;
    char jumping;
};

void clear() {
	printf("\e[1;1H\e[2J");
	return 0;
}

void render_object(struct object object) {
    printf("\033[%d;%dH", (int)round(object.y), (int)round(object.x));
    printf("\033[48;2;%d;%d;%dm", 255, 255, 0);
    printf("%c", (object.vy>=0 ? object.falling : object.jumping));
    printf("\033[48;2;%d;%d;%dm", 0, 0, 0);
}
void render_text(int x, int y, char *text) {
    printf("\033[%d;%dH%s", y, x,text);
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

LARGE_INTEGER update_deltaTime(LARGE_INTEGER lastTicks) {
    LARGE_INTEGER frequency;
    LARGE_INTEGER newTicks;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&newTicks);
    if (lastTicks.QuadPart)
        deltaTime = (float)(newTicks.QuadPart - lastTicks.QuadPart) / frequency.QuadPart;
    return newTicks;
}

int main() {
    struct object player = {5,5,0,0,0,gravity,'>','^'};
    LARGE_INTEGER lastTicks;lastTicks.QuadPart = 0;
    int i = 0;
    sleep(1);
    while (1){
        lastTicks = update_deltaTime(lastTicks);
        physics_object(&player);

        Sleep(1000/60);
        system("cls");
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
