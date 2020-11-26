#include "header.h"
int main(int argc, char *argv[]){
    /*
        A / < dan D / > untuk gerak
        Q untuk quit
        SPACE untuk shoot
    */
    // Inisialisasi Global Variable
    input = 'X';
    playerPos = MAXWIDTH/2;
    currentBullet = -1;
    
    // Create and Join Thread
    int tid;
    pthread_t userControl;
    pthread_t draw;
    pthread_create(&userControl, NULL, getInput, NULL);
    pthread_create(&draw, NULL, drawArea, NULL);
    pthread_join(userControl, NULL); // Start control thread
    pthread_join(draw, NULL); // Start draw area thread
    return 0;
}