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

#define NAMESIZE 6

char name[NAMESIZE];
int nameCursor = 0;

int snakeDirection;
int difficulty;
int fruitX, fruitY;
int points, gameRunning;
int changedDirection;

typedef struct snakePart {
    int X, Y;
    struct snakePart *prev;
    struct snakePart *next;
} SNAKEPART;

typedef struct scores {
    int score;
    char name[NAMESIZE];
} SCORES;

void drawBorders (screenColor bg);

void drawMainMenu ();

void drawScoreboardMenu();

void drawDifficultyMenu(int option);

void drawNameMenu();

void mainMenuLoop();

void difficultyLoop();

void drawName();

void nameLoop();

void scoreboardLoop();

void undrawMenu();

void readScore(SCORES *scores, int fileSize);

void writeScore();

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
    for (int i = 0; i<NAMESIZE; i++) {
        name[i] = '_';
    }
    srand(time(NULL));
    screenInit(0);
    keyboardInit();
    timerInit(100);
    mainMenuLoop();
    timerDestroy();
    keyboardDestroy();
    screenDestroy();
    return 0;
}

void gameLoop () {
    undrawMenu();
    snakeDirection = 1;
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
            drawName(RED);
            if (points == (MAXX-2)*(MAXY-2)) {
                gameWin();
                break;
            }
            drawSnake(head);
        }
        screenUpdate();
    }
    despawnSnake(head);
    while(1) {
        if(keyhit()) {
            int key = readch();
            if (key == 27) {
                break;
            }
        }
    }
    writeScore();
    undrawMenu();
    screenUpdate();
}

void readScore(SCORES *scores, int fileSize) {
    FILE *file = fopen("scores.txt", "r");
    if (file == NULL) { screenGotoxy(3,3); printf("Erro na abertura do arquivo.\n"); return; }
    for (int i = 0; i<fileSize; i++) {
        fscanf(file, "%d %s\n", &scores[i].score , scores[i].name);
    }
    fclose(file);
}

void writeScore() {
    FILE *file = fopen("scores.txt", "a");
    if (file == NULL) { screenGotoxy(3,3); printf("Erro na abertura do arquivo.\n"); return; }
    fprintf(file, "%d %s\n", points, name);
    fclose(file);
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

void drawName (screenColor bg) {
    screenSetColor(WHITE, bg);
    screenGotoxy((MAXX/2)-6,MAXY);
    printf("Nome: %s",name);
    screenSetColor(WHITE, LIGHTRED);
}
void undrawMenu() {
    // printf ("\033[2J");
    screenClear();
    drawBorders(LIGHTRED);
}

void nameLoop() {
    undrawMenu();
    int bool = 1;
    while (bool) {
        drawNameMenu();
        screenUpdate();
        if(keyhit()) {
            int key = readch();
            if (key == 27) {
                break;
            } else if (key == 127 && nameCursor > 0) {
                nameCursor--;
                name[nameCursor] = '_';
            } else if (key == '\n') {
                bool = 0;
                gameLoop();
            } else if (nameCursor < NAMESIZE && key != 127) {
                name[nameCursor] = key;
                nameCursor++;
            }
        }
    }
    undrawMenu();
    screenUpdate();
}

void drawNameMenu() {
    screenGotoxy(MAXX/2-7,MAXY/2-1);
    printf("Enter a Name:");
    screenGotoxy(MAXX/2-NAMESIZE,MAXY/2);
    for(int i = 0; i<NAMESIZE; i++) {
        if (nameCursor == i) {
            screenSetColor(BLACK,LIGHTGRAY);
            printf("%c",name[i]);
            screenSetNormal();
        } else {
            printf("%c",name[i]);
        }
        printf(" ");
    }
}

void difficultyLoop() {
    undrawMenu();
    int option = 1;
    int bool = 1;
    while (bool) {
        drawDifficultyMenu(option);
        screenUpdate();
        if(keyhit()) {
            int key = readch();
            if (key == 27) {
                break;
            } else if (key == 's') {
                if (option<3) option++;
            } else if (key == 'w') {
                if (option>1) option--;
            } else if (key == '\n') {
                switch (option) {
                    case 1: difficulty = 1; break;
                    case 2: difficulty = 2; break;
                    case 3: difficulty = 3; break;
                }
                bool = 0;
                nameLoop();
            }
        }
    }
    undrawMenu();
    screenUpdate();
}

void drawDifficultyMenu(int option) {
    screenGotoxy(MAXX/2-10,MAXY/2-3);
    printf("Choose your difficulty:");
    screenGotoxy(MAXX/2-2,MAXY/2-1);
    if (option == 1) {
        screenSetColor(BLACK,LIGHTGRAY);
        printf("Easy");
        screenSetNormal();
    }
    else {
        printf("Easy");
    }
    screenGotoxy(MAXX/2-3,MAXY/2);
    if (option == 2) {
        screenSetColor(BLACK,LIGHTGRAY);
        printf("Medium");
        screenSetNormal();
    }
    else {
        printf("Medium");
    }
    screenGotoxy(MAXX/2-2,MAXY/2+1);
    if (option == 3) {
        screenSetColor(BLACK,LIGHTGRAY);
        printf("Hard");
        screenSetNormal();
    }
    else {
        printf("Hard");
    }
}

void scoreboardLoop() {
    undrawMenu();
    while (1) {
        drawScoreboardMenu();
        screenUpdate();
        if(keyhit()) {
            int key = readch();
            if (key == 27) {
                break;
            }
        }
    }
    undrawMenu();
    screenUpdate();
}

int countLines() {
    FILE *file = fopen("scores.txt", "r");
    if (file == NULL) {
        return -1;
    }
    int lineCount = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lineCount++;
        }
    }
    fclose(file);
    return lineCount;
}

void sortScoresDescending(SCORES *scores, int fileSize) {
    for (int i = 0; i < fileSize - 1; i++) {
        for (int j = 0; j < fileSize - i - 1; j++) {
            if (scores[j].score < scores[j + 1].score) {
                SCORES temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
            }
        }
    }
}

void drawScoreboardMenu() {
    screenGotoxy(MAXX/2-5,MAXY/2-5);
    printf("Scoreboard:");
    int fileSize = countLines();
    if (fileSize <= 0) {
        screenGotoxy(MAXX/2-9,MAXY/2-3);
        printf("No Scores Detected");       
    } else {
        SCORES scores[fileSize];
        readScore(scores, fileSize);
        sortScoresDescending(scores, fileSize);
        for (int i = 0; i<fileSize && i<10; i++) {
            screenGotoxy(MAXX/2-7,MAXY/2-3+i);
            printf("%02d. %s: %03d", i+1, scores[i].name, scores[i].score);
        }
    }
}

void mainMenuLoop () {
    undrawMenu();
    int option = 1;
    int bool = 1;
    while (bool) {
        drawMainMenu(option);
        screenUpdate();
        if(keyhit()) {
            int key = readch();
            if (key == 27) {
                break;
            } else if (key == 's') {
                if (option<3) option++;
            } else if (key == 'w') {
                if (option>1) option--;
            } else if (key == '\n') {
                switch (option) {
                    case 1: difficultyLoop(); break;
                    case 2: scoreboardLoop(); break;
                    case 3: bool = 0; break;
                }
            }
        }
    }
}

void drawMainMenu (int option) {
    drawBorders(LIGHTRED);
    // ╭╮╰╯│─ ▲►▼◄
    screenSetColor(GREEN,LIGHTRED);
    screenGotoxy(MAXX/2-9,MAXY/2-6);
    printf("╭─► ╭╮▲ ╭─╮ ╷ ╱ ╭──");
    screenGotoxy(MAXX/2-9,MAXY/2-5);
    printf("╰─╮ │││ ├─┤ │╳  ├──");
    screenGotoxy(MAXX/2-9,MAXY/2-4);
    printf("──╯ ╵╰╯ ╵ ╵ ╵ ╲ ╰──");
    screenSetNormal();

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