#include <stdio.h>
#include <stdlib.h>

#include "keyboard.h"
#include "screen.h"
#include "timer.h"

int snakeLenght = 6;

typedef struct snakePart {
    int X, Y;
    struct snakePart *prev;
    struct snakePart *next;
} SNAKEPART;

void drawBorders (screenColor bg);

void drawSnake();

int main() {
    screenInit(0);
    keyboardInit();
    timerInit(100);

    drawBorders(DARKGRAY);

    while (1) {
        if(keyhit()) {
            if (readch() == 27) break;
        }
        screenUpdate();
    }
    timerDestroy();
    keyboardDestroy();
    screenDestroy();
    return 0;
}

void drawSnake () {
    // ╭╮╰╯│─ ▲►▼◄
}

void drawBorders (screenColor bg) {

    screenBoxEnable();

    screenSetColor(WHITE, bg);

    screenGotoxy(1,1);
    printf("%c",BOX_UPLEFT);
    screenGotoxy(MAXX,1);
    printf("%c",BOX_UPRIGHT);
    screenGotoxy(1,MAXY);
    printf("%c",BOX_DWNLEFT);
    screenGotoxy(MAXX,MAXY);
    printf("%c",BOX_DWNRIGHT);

    for(int i = 2; i<MAXX-1; i++) {
        screenGotoxy(i,1);
        printf("%c",BOX_HLINE);
        screenGotoxy(i,MAXY);
        printf("%c",BOX_HLINE);
    }

    for (int i = 2; i<MAXY; i++) {
        screenGotoxy(1,i);
        printf("%c",BOX_VLINE);
        screenGotoxy(MAXX,i);
        printf("%c",BOX_VLINE);
    }

    screenBoxDisable();
}

//{BLACK, RED, GREEN, BROWN, BLUE, MAGENTA, CYAN, LIGHTGRAY,DARKGRAY, LIGHTRED, LIGHTGREEN, YELLOW, LIGHTBLUE, LIGHTMAGENTA, LIGHTCYAN, WHITE}