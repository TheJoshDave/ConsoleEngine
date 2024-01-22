#include <stdio.h>

void printCharacters(int p) {
    for (int num = 0; num < (1<<p); num++) {
        int size = sizeof(num)*8;
        static char bits[] = "";
        char character = 0;
        for (int bit = 0; bit < size; bit++) {
            bits[size-1-bit] = ((num) & 1<<bit ? '1' : '0');
            character |= (bits[bit] == '1') << (size-1-bit);
        }

        printf("%d: ", num);
        printf("%s: ", bits);
        printf("%c", character);
        printf("\n");
    }
}

void printCharacter(int num) {
    int size = sizeof(num)*8;
        static char bits[] = "";
        char character = 0;
        for (int bit = 0; bit < size; bit++) {
            bits[size-1-bit] = ((num) & 1<<bit ? '1' : '0');
            character |= (bits[bit] == '1') << (size-1-bit);
        }

        printf("%d: ", num);
        printf("%s: ", bits);
        printf("%c", character);
        printf("\n");

        printf("Potential rgba:\n%d\n", (num>>0)&0xFF);
        printf("%d\n", (num>>8)&0xFF);
        printf("%d\n", (num>>16)&0xFF);
        printf("%d\n", (num>>24)&0xFF);
}

int main() {
    //printCharacters(8);
    int num = 255<<(24) | 0<<(16) | 255<<(8) | 0;
    printCharacter(0x4EBAC4);
    printCharacter(5159620);
    return 0;
}
