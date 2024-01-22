#include <stdio.h>
#include <conio.h>
#include <windows.h>
double deltaTime = 1;

LARGE_INTEGER update_deltaTime(LARGE_INTEGER lastTicks) {
    LARGE_INTEGER frequency;
    LARGE_INTEGER newTicks;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&newTicks);
    if (lastTicks.QuadPart)
        deltaTime = (double)(newTicks.QuadPart - lastTicks.QuadPart) / frequency.QuadPart;
    return newTicks;
}

int main() {
    LARGE_INTEGER lastTicks;
    lastTicks.QuadPart = 0;
    while (1){
        system("cls");
        lastTicks = update_deltaTime(lastTicks);
    }
    return 0;
}
