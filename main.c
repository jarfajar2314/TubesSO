#include "header.h"
int main(int argc, char *argv[]){
    // Backup terminal attribut
    tcgetattr(0, &old1);
    // Hide Cursor
    printf("\e[?25l");
    /*
        A / < dan D / > untuk gerak
        Q untuk quit
        SPACE untuk shoot
    */
    // Initiate all global variables
    
    // Set HighScore
    highscore = 0;
    setHighScore();
    
    // Start Screen
    int start = startScreen();

    while(start == 1)
    {
        resetAllEntity();
        input = 'X';
        playerPos = MAXWIDTH/2;
        currentBullet = -1;
        currentEnemy = -1;
        enemyOnScreen = 0;
        score = 0;
        health = 30;
        pressQ = 0;
        for(int i = 0; i < 3; i++) playTime[i] = 0;
        frame = 0;

        // Create and Join Thread
        int tid;
        pthread_t userControl;
        pthread_t draw;
        pthread_t enemiesControl;
        pthread_create(&userControl, NULL, getInput, NULL);
        pthread_create(&draw, NULL, drawArea, NULL);
        pthread_create(&enemiesControl, NULL, spawnEnemies, NULL);
        pthread_join(draw, NULL); // Join draw area thread
        
        // After exit thread show defeat screen
        int stat = defeatScreen();
        if(stat == 0) start = 0;
    }
    printf("\n");
    // Unhide cursor
    printf("\e[?25h");
    // Restore Terminal attribut
    tcsetattr(0, TCSANOW, &old1);
    return 0;
}