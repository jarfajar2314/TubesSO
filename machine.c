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
    char c;
    // As long key Q not pressed and Health higher than 0
    while(health > 0 && input != 'q')
    {   
        input = getch();
        control();
        if(input == 'q' || health <= 0)break;
    }
    // printf("Input Thread Terminated\n");
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
    // Spawn Bullet
    if(mode == SPAWN)
    {
        // Spawn
        currentBullet++;
        if(currentBullet == MAXBULLETS) currentBullet = 0; // MAX Bullet
        bullets[currentBullet].posX = playerPos;
        bullets[currentBullet].posY = MAXHEIGHT-3;
        bullets[currentBullet].show = 1;

        // If there's enemy on spawn spot
        for(int i = 0; i < MAXENEMIES; i++){
            if(enemies[i].posX == playerPos && enemies[i].posY == MAXHEIGHT-3 && enemies[i].show == 1){
                enemies[i].show = 0;
                score+=POINTS;
            }
        }
    }
    // Move Bullet
    else if(mode == MOVE){
        // All bullets on screen move upward
        for(int i = 0; i < MAXBULLETS; i++){
            if(bullets[i].show == 1) bullets[i].posY -= BULLET_SPEED;
        }
    }
}

void update()
{
    // Move bullet
    shoot(MOVE);
    // If Bullet reach the border
    if(bullets[currentBullet].posY == 0 && bullets[currentBullet].show == 1)
    {
        bullets[currentBullet].show = 0;
    }

    // Check if enemy hitted
    for(int i = 0; i < MAXBULLETS; i++){
        for(int j = 0; j < MAXENEMIES; j++){
            // If enemy hitted
            if((bullets[i].posX == enemies[j].posX) && (bullets[i].posY == enemies[j].posY) && (bullets[i].show == enemies[j].show && bullets[i].show == 1)){
                enemies[j].show = 0;
                bullets[i].show = 0;
                score+=POINTS;
            }
            // If enemy hit player
            else if((playerPos == enemies[j].posX) && (MAXHEIGHT-2 == enemies[j].posY) && (enemies[j].show == 1)){
                enemies[j].show = 0;
                health -= ENEMYDAMAGE*2;
            }
        }
    }

    if(frame % 15*4 == 0){
        for(int i = 0; i < MAXENEMIES; i++){
            // Move Enemy Downward
            if(enemies[i].show == 1) enemies[i].posY += 1;
            // If enemy reach the border
            if(enemies[i].posY == MAXHEIGHT-1 && enemies[i].show == 1)
            {
                enemies[i].show = 0;
                health -= ENEMYDAMAGE;
            }
        }
    }
}

// Check if bullet exist in x and y coordinates
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

// Return random number between min and max
int getRandom(int min, int max)
{
    return (rand() % (max - min + 1)) + min; 
}

void *spawnEnemies(void *vargp)
{
    int c = 0;
    int spawnTime = 30; // spawn enemies every x/10 sec
    while(1)
    {   
        // Count Enemy On Screen
        enemyOnScreen = 0;
        for(int i = 0; i < MAXENEMIES; i++){
            if(enemies[i].show == 1) enemyOnScreen++;
        }
        // Spawn Enemy every spawnTime
        if(c % spawnTime == 0 && enemyOnScreen <= MAXENEMIES){
            currentEnemy++;
            if(currentEnemy == MAXENEMIES) currentEnemy = 0; // MAX Bullet
            srand(time(0));
            enemies[currentEnemy].posX = getRandom(1, MAXWIDTH-2);
            enemies[currentEnemy].posY = getRandom(1, 8);
            enemies[currentEnemy].show = 1;
        }
        // If user press 'Q' or health lower than 0
        if(input == 'q' || health <= 0)break;
        msleep(100);
        c++;
    }
    // printf("Enemy Thread Terminated\n");
    pthread_exit(NULL);
}

// Check if enemy exist in x and y coordinates
int checkEnemy(int x, int y)
{
    int check = 0;
    for(int i = 0; i < MAXENEMIES; i++){
        if(enemies[i].posX == x && enemies[i].posY == y && enemies[i].show == 1){
            check = 1;
            break;
        }
    }
    return check;
}

// Print area / windows
void *drawArea(void *vargp)
{
    while(1)
    {
        clearScreen();
        for(int i = 0; i < MAXHEIGHT; i++)
        {
            for(int j = 0; j < MAXWIDTH; j++)
            {
                // Print Arena Border
                if((i == 0 && j == 0) || (i == 0 && j == MAXWIDTH-1) || (i == MAXHEIGHT-1 && j == 0) || (i == MAXHEIGHT-1 && j == MAXWIDTH-1))
                {
                    if(i == j && i == 0) printf("\u2554");
                    else if(i == 0 && j == MAXWIDTH-1) printf("\u2557");
                    else if(j == 0 && i == MAXHEIGHT-1) printf("\u255A");
                    else printf("\u255D");
                }
                // Print Arena Border
                else if(i == 0 || i == MAXHEIGHT-1)
                {
                    printf("\u2550");
                }
                // Print Arena Border
                else if((i > 0 && i < MAXHEIGHT-1) && (j == 0 || j == MAXWIDTH-1))
                {
                    printf("\u2551");
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
                    // Print Enemy
                    else if(checkEnemy(j, i) == 1)
                    {
                        RED(1);
                        printf("V");
                        RESETCOLOR();
                    }
                    // Print Player
                    else if(i == MAXHEIGHT-2 && (j == playerPos))
                    {
                        CYAN(0);
                        printf("\u0394");
                        RESETCOLOR();
                    }
                    // Print blank space
                    else printf(" ");
                }
            }
            // Display Time
            if(i == 1)printTime();
            // Display Score
            if(i == 2)printf("Score : %d", score);
            printf("\n");
        }
        // Display Health
        printf(" Health : ");
        GREEN(0);
        for(int i = 0; i < health; i++) printf("\u2588");
        RESETCOLOR();
        printf("\n");
        // ===============
        // for debugging
        // printf("FPS:%d | PPos:%d | cBullet:%d [%d,%d]%d | cEnemy %d[%d,%d]%d | Health:%d\n", FPS, playerPos, currentBullet, bullets[currentBullet].posX, bullets[currentBullet].posY, bullets[currentBullet].show, currentEnemy, enemies[currentEnemy].posX, enemies[currentEnemy].posY, enemyOnScreen, health);
        // ==============
        // If user press 'Q' or health lower than 0, break the loop, exit thread
        if(input == 'q' || health <= 0)break;
        // Frame Count
        ++frame;
        // Update position of bullet, enemy status, etc
        update();
        // Time of Sleep
        fps(FPS);
    }
    // printf("Draw Thread Terminated\n");
    pthread_exit(NULL);
}

// Print Play Time
void printTime()
{
    printf("%02.0f:%02.0f", (float)playTime[1], (float)playTime[2]);
    if(frame % 15 == 0) ++playTime[2];
    if(playTime[2] == 60) playTime[2] = 0, ++playTime[1];
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

void resetAllEntity()
{
    for(int i = 0; i < MAXBULLETS; i++){
        bullets[i].posX = 0;
        bullets[i].posY = 0;
        bullets[i].show = 0;
    }
    for(int i = 0; i < MAXENEMIES; i++){
        enemies[i].posX = 0;
        enemies[i].posY = 0;
        enemies[i].show = 0;
    }
}

// Start Screen
void startScreen()
{
    char i = '-';
    while(i == '-'){
        system("clear");
        printf("   _____                    _____ _           _           \n");
        printf("  |   __|___ ___ ___ ___   |   __| |_ ___ ___| |_ ___ ___ \n");
        printf("  |__   | . | .'|  _| -_|  |__   |   | . | . |  _| -_|  _|\n");
        printf("  |_____|  _|__,|___|___|  |_____|_|_|___|___|_| |___|_|  \n");
        printf("        |_|                                               \n\n");
        msleep(500);
        printf("                    +-----------------+                 \n");
        printf("                       HIGHSCORE %d                     \n", highscore);
        printf("                    +-----------------+                 \n\n");
        printf("                   Press any key to start.              \n");
        i = getch();
    }
}

// Defeat Screen
int defeatScreen()
{
    setHighScore();
    if(score > highscore) highscore = score;
    char input;
    system("clear");
    printf("  ______   _______ _______ _______ _______ _______ \n");
    printf(" |   _  \\ |   _   |   _   |   _   |   _   |       |\n");
    printf(" |.  |   \55|.  1___|.  1___|.  1___|.  1   |.|   | |\n");
    printf(" |.  |    |.  __)_|.  __) |.  __)_|.  _   `-|.  |-'\n");
    printf(" |:  1    |:  1   |:  |   |:  1   |:  |   | |:  |  \n");
    printf(" |::.. . /|::.. . |::.|   |::.. . |::.|:. | |::.|  \n");
    printf(" `------' `-------`---'   `-------`--- ---' `---'  \n\n");
	msleep(500);
	printf("                 SCORE : %d              \n", score);
    printf("               +-----------------+       \n");
    printf("                 HIGHSCORE %d            \n", highscore);
    printf("               +-----------------+       \n\n");
    msleep(500);
    printf("  Press Enter to Try Again or Any Key to Exit\n\n");
    input = getch();
    if(input == 10) return 1;
    else return 0;
}

void setHighScore()
{
    FILE *data;
    data = fopen("data.txt", "r");
    // Write Data
    if(data == NULL || score > highscore){
        data = fopen("data.txt", "w");
        fprintf(data, "%s %d", "HighScore", score);
        fclose(data);
    }
    // Read Data
    else{
        char string[20];
        fscanf(data, "%s %d\n", string, &highscore);
        fclose(data);
    }
}

/*
Λ 039B
Δ 0394
∀ 2200
*/