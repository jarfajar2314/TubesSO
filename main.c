#include "header.h"
int main(int argc, char *argv[]){
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
    startScreen();

    while(1)
    {
        resetAllEntity();
        input = 'X';
        playerPos = MAXWIDTH/2;
        currentBullet = -1;
        currentEnemy = -1;
        enemyOnScreen = 0;
        score = 0;
        health = 30;
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
        if(stat == 0) break;
    }

    return 0;
}