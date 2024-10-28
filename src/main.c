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

void drawSnake(SNAKEPART *snakeHead);

void addSnake(SNAKEPART *snakeHead, int x, int y);

SNAKEPART *spawnSnake();

void despawnSnake(SNAKEPART *snakeHead);

int main() {
    screenInit(0);
    keyboardInit();
    timerInit(100);

    drawBorders(DARKGRAY);
    SNAKEPART *head = spawnSnake();

    drawSnake(head);
    
    while (1) {
        if(keyhit()) {
            if (readch() == 27) break;
        }
        screenUpdate();
    }

    despawnSnake(head);
    timerDestroy();
    keyboardDestroy();
    screenDestroy();
    return 0;
}

SNAKEPART *spawnSnake() {
    SNAKEPART *head = malloc(sizeof(SNAKEPART));
    head->X = (MAXX/2)+1;
    head->Y = (MAXY/2);
    head->prev = NULL;
    head->next = NULL;
    addSnake(head, MAXX/2, MAXY/2);
    addSnake(head, (MAXX/2)-1, (MAXY/2));
    return head;
}

void despawnSnake(SNAKEPART *snakeHead) {
    free(snakeHead);
}

void addSnake(SNAKEPART *snakeHead, int x, int y) {
    SNAKEPART *temp = snakeHead->next;
    while(temp != NULL) {
        snakeHead = temp;
        temp = temp->next;
    }
    snakeHead->next = malloc(sizeof(SNAKEPART));
    temp = snakeHead->next;
    temp->X = x;
    temp->Y = y;
    temp->prev = snakeHead;
    temp->next = NULL;
}

void drawSnake(SNAKEPART *snakeHead) {
    // ╭╮╰╯│─ ▲►▼◄
    screenGotoxy(snakeHead->X, snakeHead->Y);
    printf("►");
    SNAKEPART *temp = snakeHead->next;
    while(temp != NULL) {
        screenGotoxy(temp->X,temp->Y);
        printf("─");
        temp = temp->next;
    }
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