/**
 * main.c
 * Created on Oct, 10th 2024
 * Author: Rafael Varela
 * Based on "Ozymandias"
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "keyboard.h"
#include "screen.h"
#include "timer.h"

int snakeLenght = 3;
int snakeDirection = 1;
int difficulty = 1;
int fruitX, fruitY;
int points, gameRunning;
int changedDirection;

typedef struct snakePart {
    int X, Y;
    struct snakePart *prev;
    struct snakePart *next;
} SNAKEPART;

void drawBorders (screenColor bg);

void drawMainMenu ();

// to be separeted

void gameLoop ();

void drawPoints (screenColor bg);

void drawSnake(SNAKEPART *snakeHead);

void undrawSnake(SNAKEPART *snakeHead);

void addSnake(SNAKEPART *snakeHead, int x, int y);

void moveSnake(SNAKEPART *snakeHead);

void collisionFruit(SNAKEPART *snakeHead);

void collisionWall(SNAKEPART *snakeHead);

void collisionSnake(SNAKEPART *snakeHead);

void spawnFruit(SNAKEPART *snakeHead);

SNAKEPART *spawnSnake();

void despawnSnake(SNAKEPART *snakeHead);

void inputHandler(int ch);

void gameOver();

void gameWin();

int main() {
    srand(time(NULL));
    screenInit(0);
    keyboardInit();
    timerInit(100);

    mainMenuLoop();
    gameLoop();
    while(1) {
        if(keyhit()) {
            int key = readch();
            if (key == 27) {
                break;
            }
        }
    }
    timerDestroy();
    keyboardDestroy();
    screenDestroy();
    return 0;
}

void gameLoop () {
    points = 0;
    drawBorders(RED);
    SNAKEPART *head = spawnSnake();
    spawnFruit(head);
    gameRunning = 1, changedDirection = 1;
    while (1) {
        if(keyhit()) {
            int key = readch();
            if (key == 27) {
                break;
            } else {
                inputHandler(key);
            }
        }
        if(getTimeDiff() > 100/difficulty) {
            timerUpdateTimer(1000);
            screenGotoxy(2,3);
            moveSnake(head);
            if (!gameRunning) {
                screenUpdate();
                break;
            }
            drawPoints(RED);
            if (points == (MAXX-2)*(MAXY-2)) {
                gameWin();
                break;
            }
            drawSnake(head);
        }
        screenUpdate();
    }
    despawnSnake(head);
}

void spawnFruit(SNAKEPART *snakeHead) {
    int i;
    SNAKEPART *temp;
    while(i) {
        temp = snakeHead;
        i = 0;
        fruitX = rand() % (MAXX - 3) + 2;
        fruitY = rand() % (MAXY - 3) + 2;
        while(temp->next != NULL) {
            if(temp->X == fruitX && temp->Y == fruitY) {
                i = 1;
                break;
            }
            temp = temp->next;
        }
    }
    screenGotoxy(fruitX, fruitY);
    screenSetColor(RED, LIGHTRED);
    printf("Ó");
    screenSetColor(WHITE, LIGHTRED);
}

SNAKEPART *spawnSnake() {
    SNAKEPART *head = malloc(sizeof(SNAKEPART));
    head->X = (MAXX/2)+1;
    head->Y = (MAXY/2);
    head->prev = NULL;
    head->next = NULL;
    addSnake(head, MAXX/2, MAXY/2);
    addSnake(head, MAXX/2-1, MAXY/2);
    addSnake(head, (MAXX/2)-2, (MAXY/2));
    return head;
}

void despawnSnake(SNAKEPART *snakeHead) {
    SNAKEPART *temp;
    while(snakeHead != NULL) {
        temp = snakeHead;
        snakeHead = snakeHead->next;
        free(temp);
    }
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

void collisionFruit(SNAKEPART *snakeHead) {
    int x = snakeHead->X, y = snakeHead->Y;
    switch (snakeDirection) {
        case 0: y--; break;
        case 1: x++; break;
        case 2: y++; break;
        case 3: x--; break;
    }
    if (x == fruitX && y == fruitY) {
        spawnFruit(snakeHead);
        SNAKEPART *temp = snakeHead->next;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        x = temp->X;
        y = temp->Y;
        points++;
        addSnake(temp, x, y);
    }
}

void collisionWall(SNAKEPART *snakeHead) {
    int x = snakeHead->X, y = snakeHead->Y;
    switch (snakeDirection) {
        case 0: y--; break;
        case 1: x++; break;
        case 2: y++; break;
        case 3: x--; break;
    }
    if(x == 1 || y == 1 || x == MAXX-1 || y == MAXY) {
        gameOver();
    }
}

void collisionSnake(SNAKEPART *snakeHead) {
    if(points > 1) {
        int x = snakeHead->X, y = snakeHead->Y;
        switch (snakeDirection) {
            case 0: y--; break;
            case 1: x++; break;
            case 2: y++; break;
            case 3: x--; break;
        }
        SNAKEPART *temp = snakeHead->next;
        while(temp->next != NULL) {
            if(temp->X == x && temp->Y == y) {
                gameOver();
                break;
            }
            temp = temp->next;
        }
    }
}

void moveSnake(SNAKEPART *snakeHead) {
    changedDirection = 1;
    collisionWall(snakeHead);
    collisionSnake(snakeHead);
    collisionFruit(snakeHead);
    int x = snakeHead->X, y = snakeHead->Y, t;
    undrawSnake(snakeHead);
    SNAKEPART *temp = snakeHead->next;
    while(temp != NULL) {
        t = temp->X;
        temp->X = x;
        x = t;
        t = temp->Y;
        temp->Y = y;
        y = t;
        temp = temp->next;
    }
    switch (snakeDirection) {
        case 0: snakeHead->Y--; break;
        case 1: snakeHead->X++; break;
        case 2: snakeHead->Y++; break;
        case 3: snakeHead->X--; break;
    }
}

void undrawSnake(SNAKEPART *snakeHead) {
    // ╭╮╰╯│─ ▲►▼◄
    screenGotoxy(snakeHead->X, snakeHead->Y);
    printf(" ");
    SNAKEPART *temp = snakeHead->next;
    while(temp != NULL) {
        screenGotoxy(temp->X,temp->Y);
        printf(" ");
        temp = temp->next;
    }
}

void drawSnake(SNAKEPART *snakeHead) {
    // ╭╮╰╯│─ ▲►▼◄
    screenSetColor(LIGHTGREEN, LIGHTRED);

    screenGotoxy(snakeHead->X, snakeHead->Y);
    switch (snakeDirection) {
        case 0: printf("▲"); break;
        case 1: printf("►"); break;
        case 2: printf("▼"); break;
        case 3: printf("◄"); break;
    }
    SNAKEPART *temp = snakeHead->next;
    SNAKEPART *tempNext = temp->next;
    int x1, y1, x2, y2;
    while(temp->next != NULL) {
        screenSetColor(LIGHTGREEN, LIGHTRED);
        screenGotoxy(temp->X,temp->Y);
        x1 = snakeHead->X - temp->X;
        y1 = snakeHead->Y - temp->Y;
        x2 = tempNext->X - temp->X;
        y2 = tempNext->Y - temp->Y;
        if ((x1 == -1 && y1 == 0 && x2 == 1 && y2 == 0) || (x1 == 1 && y1 == 0 && x2 == -1 && y2 == 0)) {
            printf("─");
        } else if ((x1 == 0 && y1 == -1 && x2 == 0 && y2 == 1) || (x1 == 0 && y1 == 1 && x2 == 0 && y2 == -1)) {
            printf("│");
        } else if ((x1 == 0 && y1 == -1 && x2 == 1 && y2 == 0) || (x1 == 1 && y1 == 0 && x2 == 0 && y2 == -1)) {
            printf("╰");
        } else if ((x1 == -1 && y1 == 0 && x2 == 0 && y2 == -1) || (x1 == 0 && y1 == -1 && x2 == -1 && y2 == 0)) {
            printf("╯");
        } else if ((x1 == -1 && y1 == 0 && x2 == 0 && y2 == 1) || (x1 == 0 && y1 == 1 && x2 == -1 && y2 == 0)) {
            printf("╮");
        } else if ((x1 == 0 && y1 == 1 && x2 == 1 && y2 == 0) || (x1 == 1 && y1 == 0 && x2 == 0 && y2 == 1)) {
            printf("╭");
        } else {
            printf("O");
        } 
        snakeHead = snakeHead->next;
        tempNext = tempNext->next;
        temp = temp->next;
    }
    // screenGotoxy(temp->X,temp->Y);
    // if(temp->X - snakeHead->X != 0) {
    //     printf("─");
    // } else {
    //     printf("│");
    // }
    screenSetColor(WHITE, LIGHTRED);

}

void inputHandler(int ch) {
    if (changedDirection) {
        switch (ch) {
            case 'w': if (snakeDirection != 2) snakeDirection = 0; break;
            case 'd': if (snakeDirection != 3) snakeDirection = 1; break;
            case 's': if (snakeDirection != 0) snakeDirection = 2; break;
            case 'a': if (snakeDirection != 1) snakeDirection = 3; break;
        }
        changedDirection = 0;
    }
}

void gameOver() {
    screenGotoxy(MAXX/2-5, MAXY/2);
    screenSetColor(WHITE, RED);
    printf("Game Over!");
    screenGotoxy(MAXX/2-5, MAXY/2+1);
    printf("- %04d Ó -",points);
    screenSetColor(WHITE, LIGHTRED);
    screenGotoxy(fruitX, fruitY);
    printf(" ");
    gameRunning = 0;
}

void gameWin() {
    screenSetColor(WHITE, GREEN);
    screenGotoxy(MAXX/2-5, MAXY/2);
    printf("Você ganhou");
    screenSetColor(WHITE, LIGHTRED);
}

void drawPoints (screenColor bg) {
    screenSetColor(WHITE, bg);
    screenGotoxy((MAXX/2)-7,1);
    printf("Pontuação: %03d",points);
    screenSetColor(WHITE, LIGHTRED);
}

void undrawMenu() {
    screenClear();
    drawBorders(LIGHTRED);
}

void nameLoop() {
    undrawMenu();
}

void difficultyLoop() {
    undrawMenu();
}

void scoreboardLoop() {
    undrawMenu();
}

void mainMenuLoop () {
    undrawMenu();
    int option = 1;
    int bool = 1;
    while (bool) {
        if(keyhit()) {
            int key = readch();
            if (key == 27) {
                break;
            } else if (key == 's') {
                if (option<3) option++;
            } else if (key == 'w') {
                if (option>1) option--;
            } else if (key == 13) {
                switch (option) {
                    case 1: difficultyLoop(); break;
                    case 2: scoreboardLoop(); break;
                    case 3: bool = 0; break;
                }
            }
        }
        drawMainMenu(option);
        screenUpdate();
    }
}

void drawMainMenu (int option) {
    drawBorders(LIGHTRED);
    screenGotoxy(MAXX/2-2,MAXY/2-1);
    if (option == 1) {
        screenSetColor(BLACK,LIGHTGRAY);
        printf("Play");
        screenSetNormal();
    }
    else {
        printf("Play");
    }
    screenGotoxy(MAXX/2-5,MAXY/2);
    if (option == 2) {
        screenSetColor(BLACK,LIGHTGRAY);
        printf("Scoreboard");
        screenSetNormal();
    }
    else {
        printf("Scoreboard");
    }
    screenGotoxy(MAXX/2-2,MAXY/2+1);
    if (option == 3) {
        screenSetColor(BLACK,LIGHTGRAY);
        printf("Exit");
        screenSetNormal();
    }
    else {
        printf("Exit");
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
    //reminder to self LIGHTRED a.k.a 39 as bg is default terminal color. https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
    screenSetColor(WHITE, LIGHTRED);
}

//{BLACK, RED, GREEN, BROWN, BLUE, MAGENTA, CYAN, LIGHTGRAY,DARKGRAY, LIGHTRED, LIGHTGREEN, YELLOW, LIGHTBLUE, LIGHTMAGENTA, LIGHTCYAN, WHITE}