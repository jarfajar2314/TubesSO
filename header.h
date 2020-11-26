#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <time.h>
#include <errno.h>

#define PLAYING 1
#define clearScreen() printf("\e[1;1H\e[2J");
// Text Color Macro
#define RED(bold) printf("\033[%d;31m", bold);
#define GREEN(bold) printf("\033[%d;32m", bold);
#define YELLOW(bold) printf("\033[%d;33m", bold);
#define BLUE(bold) printf("\033[%d;34m", bold);
#define MAGENTA(bold) printf("\033[%d;35m", bold);
#define CYAN(bold) printf("\033[%d;36m", bold);
#define RESETCOLOR() printf("\033[0m");

#define MAXWIDTH 47
#define MAXHEIGHT 23
#define FPS 15
#define MAXBULLETS 20
#define SPAWN 0
#define MOVE 1
#define BULLET_SPEED 1
// < 68, > 67, a 97, d 100, A 56, D 68
// Pressed Key Macro
#define ARROW_LEFT 68
#define ARROW_RIGHT 67
#define K_LEFT 97
#define K_RIGHT 100
#define K_SPACE 32
#define PLAYER 202

char input;
int playerPos;
// Bullet struct
typedef struct{
    int posX;
    int posY;
    int show;
}bullet;
// Array of Bullets
bullet bullets[MAXBULLETS];
// Bullet iterator
int currentBullet;

// Create getch func
static struct termios old, current;
void initTermios(int echo) ;
void resetTermios(void);
char getch_(int echo);
char getch(void);
char getche(void);
// ===================

void *getInput(void *vargp); // Input from User
void *drawArea(void *vargp); // Draw area
int msleep(long msec); // Milisecond Sleep
void control(); // control User Character
void fps(int frame); // frame per second
void shoot(int mode);
void update();
int checkBullet(int x, int y);