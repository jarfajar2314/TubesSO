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
        if(input == 'q')pressQ = 1;
        control();
        if(input == 'q' || health <= 0)break;
    }
    // printf("Input Thread Terminated\n");
    pthread_exit(NULL);
}

void control()
{
    // Player Move
    if(((input == K_LEFT) || (input == ARROW_LEFT)) && (playerPos > 2)) playerPos--;
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
        gotoxy(bullets[currentBullet].posX, 0);
        printf("\u2550");
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
            enemies[currentEnemy].posX = getRandom(2, MAXWIDTH-2);
            enemies[currentEnemy].posY = getRandom(2, 8);
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

// Draw border first
void drawBorder()
{
    clearScreen();
    for(int i = 0; i < MAXHEIGHT-1; i++)
    {
        for(int j = 0; j < MAXWIDTH; j++)
        {
            // Print Arena Border
            if((i == 0 && j == 0) || (i == 0 && j == MAXWIDTH-1) || (i == MAXHEIGHT-2 && j == 0) || (i == MAXHEIGHT-2 && j == MAXWIDTH-1))
            {
                if(i == j && i == 0) printf("\u2554"); // ╔
                else if(i == 0 && j == MAXWIDTH-1) printf("\u2557"); // ╗
                else if(j == 0 && i == MAXHEIGHT-2) printf("\u255A"); // ╚ 
                else printf("\u255D"); // ╝
            }
            // Print Arena Border
            else if(i == 0 || i == MAXHEIGHT-2)
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
                printf(" ");
            }
        }
        printf("\n");
    }
}

// Clear the arena
void clearArena()
{
    gotoxy(0, 0);
    for(int i = 1; i < MAXHEIGHT-2; i++){
        gotoxy(2, i+1);
        for(int j = 0; j < MAXWIDTH-2; j++){
            printf(" ");
        }
    }
    gotoxy(0, 0);
}

// Print area / windows
void *drawArea(void *vargp)
{
    drawBorder();
    while(1)
    {
        // clearScreen();
        clearArena();
        // gotoxy(0, 0);
        for(int i = 0; i < MAXHEIGHT; i++)
        {
            for(int j = 0; j < MAXWIDTH; j++)
            {
                // Print Bullets
                if(checkBullet(j, i) == 1 && i > 1)
                {
                    YELLOW(0);
                    gotoxy(j, i);
                    printf("|");
                    RESETCOLOR();
                }
                // Print Enemies
                else if(checkEnemy(j, i) == 1 && j > 1 && i > 1)
                {
                    RED(1);
                    gotoxy(j, i);
                    printf("V");
                    RESETCOLOR();
                }
                // Print Player
                else if(i == MAXHEIGHT-2 && (j == playerPos) && j > 1)
                {
                    CYAN(0);
                    gotoxy(j, i);
                    printf("\u0394");
                    RESETCOLOR();
                }
                gotoxy(0,0);
            }
            // Display Time
            if(i == 1)
            gotoxy(MAXWIDTH+1, 2),
            printTime();
            // Display Score
            if(i == 2)
            gotoxy(MAXWIDTH+1, 3),
            printf("Score : %d", score);
            printf("\n");
        }
        // Display Health
        gotoxy(1, MAXHEIGHT+1);
        printf(" Health : ");
        GREEN(0);
        printf("                              ");
        gotoxy(11, MAXHEIGHT+1);
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

// Move cursor
void gotoxy(int x, int y)
{
    printf("%c[%d;%df", 0x1B, y, x);
}

// Start Screen
int startScreen()
{
        system("clear");
        printf("      _______                        \n");  
		printf("     |   _   .-----.---.-.----.-----.\n");
		printf("     |   1___|  _  |  _  |  __|  -__|\n");
		printf("     |____   |   __|___._|____|_____|\n");
		printf("     |:  1   |__|                    \n");                                
		printf("     `-------'  						\n");
		printf(" _______ __                __              \n");                     
		printf("|   _   |  |--.-----.-----|  |_.-----.----.\n");
		printf("|   1___|     |  _  |  _  |   _|  -__|   _|\n");													
		printf("|____   |__|__|_____|_____|____|_____|__|  \n");
		printf("|:  1   |							       \n");
		printf("`-------'								   \n");        
        msleep(500);
        printf("            +-----------------+          \n");
        printf("                HIGHSCORE %d             \n", highscore);
        printf("            +-----------------+          \n\n");
    int i = 0;
    int play = 0;
    int selection = 1;
    while(play == 0){
        gotoxy(0, 17);
        printf("     ");
        gotoxy(0, 17);
        if(selection == 1) GREEN(1);
        printf("                    PLAY                            \n\n");
        if(selection == 1) RESETCOLOR();

        gotoxy(0, 19);
        printf("                               ");
        gotoxy(0, 19);
        if(selection == 2) GREEN(1);
        printf("                    QUIT                            \n\n");
        if(selection == 2) RESETCOLOR();
        i = getch();
        switch (i)
        {
            case ARROW_UP:
                if(selection == 1) selection = 2;
                else selection--;
                break;

            case ARROW_DOWN:
                if(selection == 2) selection = 1;
                else selection++;
                break;
            case K_ENTER:
                if(selection == 1) play = 1;
                else if(selection == 2){
                    return 0;
                }
                break;
            default:
                break;
        }
    }
    return 1;
}

// Defeat Screen
int defeatScreen()
{
    char input;
    system("clear");
    setHighScore();
    if(score > highscore)
    {
        highscore = score;
		printf(" __ __  ____    __ ______   ___   ____   __ __ \n");
		printf("|  T  |l    j  /  ]      T /   \\ |    \\ |  T  T\n");
		printf("|  |  | |  T  /  /|      |Y     Y|  D  )|  |  |\n");
		printf("|  |  | |  | /  / l_j  l_j|  O  ||    / |  ~  |\n");
		printf("l  :  ! |  |/   \\_  |  |  |     ||    \\ l___, |\n");
		printf(" \\   /  j  l\\     | |  |  l     !|  .  Y|     !\n");
		printf("  \\_/  |____j\\____j l__j   \\___/ l__j _jl____/ \n\n");
        printf("                HIGHSCORE UPDATED               \n");
    }
    else if(score < highscore)
    {
        printf("  ______   _______ _______ _______ _______ _______ \n");
        printf(" |   _  \\ |   _   |   _   |   _   |   _   |       |\n");
        printf(" |.  |   \55|.  1___|.  1___|.  1___|.  1   |.|   | |\n");
        printf(" |.  |    |.  __)_|.  __) |.  __)_|.  _   `-|.  |-'\n");
        printf(" |:  1    |:  1   |:  |   |:  1   |:  |   | |:  |  \n");
        printf(" |::.. . /|::.. . |::.|   |::.. . |::.|:. | |::.|  \n");
        printf(" `------' `-------`---'   `-------`--- ---' `---'  \n\n");
    }
        msleep(500);
        printf("                    SCORE : %d           \n", score);
        printf("               +-----------------+       \n");
        printf("                  HIGHSCORE %d           \n", highscore);
        printf("               +-----------------+       \n");
        msleep(500);
        if(pressQ == 0)
        printf("             PRESS ENTER TO CONTINUE             \n\n");
    int slc = 1;
    while(1){
        
        gotoxy(15, 15);
        if(slc == 1) GREEN(1);
        printf("Try Again");
        if(slc == 1) RESETCOLOR();
        
        gotoxy(29, 15);
        if(slc == 2) GREEN(1);
        printf("Exit");
        if(slc == 2) RESETCOLOR();
        input = getch();
        if(input == ARROW_RIGHT){
            if(slc == 2) slc = 1;
            else slc++;
        }
        else if(input == ARROW_LEFT){
            if(slc == 1) slc = 2;
            else slc--;
        }
        else if(input == K_ENTER){
            if(slc == 1) return 1;
            else if(slc == 2) return 0;
        }
        else{
        }
    }
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