#include "header.h"

// ((((( BUAT GETCH)))))
/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  current = old; /* make new settings same as old settings */
  current.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
      current.c_lflag |= ECHO; /* set echo mode */
  } else {
      current.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &current); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void) 
{
  return getch_(1);
}
// =================

// Read Keyboard input
void *getInput(void *vargp)
{
    clearScreen();
    // printf("Start\n");
    char c;
    while(PLAYING)
    {
        // printf("\e[?25l"); // Remove Cursor
        input = getch();
        control();
        // printf("\nTyped:%c | %d\n", input, input);
        // fflush(stdout);
        if(input == 'q') break;
    }
    pthread_exit(NULL);
}

void control()
{
    // Player Move
    if(((input == K_LEFT) || (input == ARROW_LEFT)) && (playerPos != 1)) playerPos--;
    if(((input == K_RIGHT) || (input == ARROW_RIGHT)) && (playerPos != MAXWIDTH-2)) playerPos++;

    // Player Shooting
    if(input == K_SPACE)shoot(SPAWN);
}

void shoot(int mode)
{
    if(mode == SPAWN)
    {
        // Inisiasi
        currentBullet++;
        if(currentBullet == MAXBULLETS) currentBullet = 0; // MAX Bullet
        bullets[currentBullet].posX = playerPos;
        bullets[currentBullet].posY = MAXHEIGHT-3;
        bullets[currentBullet].show = 1;
    }
    else if(mode == MOVE){
        // Bullets moving
        for(int i = 0; i < MAXBULLETS; i++){
            if(bullets[i].show == 1) bullets[i].posY -= BULLET_SPEED;
        }
    }
}

void update()
{
    shoot(MOVE);
    // If Bullet reach end of windows
    if(bullets[currentBullet].posY == 0 && bullets[currentBullet].show == 1)
    {
        bullets[currentBullet].show = 0;
    }
}

// Cek jika ada bullet di koordinat x, y
int checkBullet(int x, int y)
{
    int check = 0;
    for(int i = 0; i < MAXBULLETS; i++){
        if(bullets[i].posX == x && bullets[i].posY == y && bullets[i].show == 1){
            check = 1;
            break;
        }
    }
    return check;
}

// Print area / windows
void *drawArea(void *vargp)
{
    while(PLAYING)
    {
        clearScreen();
        for(int i = 0; i < MAXHEIGHT; i++)
        {
            for(int j = 0; j < MAXWIDTH; j++)
            {
                if((i == 0 && j == 0) || (i == 0 && j == MAXWIDTH-1) || (i == MAXHEIGHT-1 && j == 0) || (i == MAXHEIGHT-1 && j == MAXWIDTH-1))
                {
                    if(i == j && i == 0) printf("\u2554");
                    else if(i == 0 && j == MAXWIDTH-1) printf("\u2557");
                    else if(j == 0 && i == MAXHEIGHT-1) printf("\u255A");
                    else printf("\u255D");
                    // printf("+");
                    fflush(stdout);
                }
                else if(i == 0 || i == MAXHEIGHT-1)
                {
                    printf("\u2550");
                    fflush(stdout);
                }
                else if((i > 0 && i < MAXHEIGHT-1) && (j == 0 || j == MAXWIDTH-1))
                {
                    printf("\u2551");
                    fflush(stdout);
                }
                else
                {
                    // Print Bullet
                    if(checkBullet(j, i) == 1)
                    {
                        YELLOW(0);
                        printf("|");
                        RESETCOLOR();
                    } 
                    // Print Player
                    else if(i == MAXHEIGHT-2 && (j == playerPos))
                    {
                        CYAN(0);
                        printf("\u0394");
                        // printf("\u23F0");
                        // printf("%d", input);
                        RESETCOLOR();
                    }
                    // Print blank space
                    else printf(" ");
                    fflush(stdout);
                }
            }
            printf("\n");
            fflush(stdout);
        }
        // for debugging
        printf("FPS:%d | PPos:%d | cBullet:%d [%d,%d]%d\n", FPS, playerPos, currentBullet, bullets[currentBullet].posX, bullets[currentBullet].posY, bullets[currentBullet].show);
        // ==============
        if(input == 'q')break;
        update();
        fps(FPS);
    }
    pthread_exit(NULL);
}

void fps(int frame)
{
    int f = 1000/frame;
    msleep(f);
}

// Sleep in Milisecond
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

/*
Λ 039B
Δ 0394
∀ 2200
*/