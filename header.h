#include <stdio.h> // standard functions
#include <stdlib.h> // srand(), rand()
#include <unistd.h> // sleep()
#include <pthread.h> // all thread func
#include <termios.h> // all termios func
#include <time.h> // time()
#include <errno.h> // used at msleep() func

// #define clearScreen() system("clear");
// #define clearScreen() printf("\e[1;1H\e[2J"), fflush(stdout);
#define clearScreen() printf("\033[2J\033[1;1H");
// Text Color Macro
#define RED(bold) printf("\033[%d;31m", bold);
#define GREEN(bold) printf("\033[%d;32m", bold);
#define YELLOW(bold) printf("\033[%d;33m", bold);
#define BLUE(bold) printf("\033[%d;34m", bold);
#define MAGENTA(bold) printf("\033[%d;35m", bold);
#define CYAN(bold) printf("\033[%d;36m", bold);
#define RESETCOLOR() printf("\033[0m");

#define MAXWIDTH 43
#define MAXHEIGHT 21
#define FPS 15
#define MAXBULLETS 20
#define SPAWN 0
#define MOVE 1
#define BULLET_SPEED 1
#define MAXENEMIES 10
#define POINTS 10
#define ENEMYDAMAGE 5
// < 68, > 67, a 97, d 100, A 56, D 68
// Pressed Key Macro
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_LEFT 68
#define ARROW_RIGHT 67
#define K_LEFT 97
#define K_RIGHT 100
#define K_SPACE 32
#define K_ENTER 10
#define PLAYER 202

char input;
int playerPos;
int playTime[3];
int frame;
int score;
int highscore;
int health;
int pressQ;
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

// Enemy Struct
typedef struct{
    int posX;
    int posY;
    int show;
}enemy;

// Array of Enemies
enemy enemies[MAXENEMIES];
// Enemies Iterator
int currentEnemy;
// On Sreen Enemy Counter
int enemyOnScreen;

// Create getch func
static struct termios old, current, old1;
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
int checkEnemy(int x, int y);
void *spawnEnemies(void *vargp);
int getRandom(int min, int max);
void printTime();
int startScreen();
int defeatScreen();
void setHighScore();
void resetAllEntity();
void gotoxy(int x, int y);
void drawBorder();
void clearArena();