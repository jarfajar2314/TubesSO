#include "header.h"
int main(int argc, char *argv[]){
    // Inisialisasi Global Variable
    input = 'X';
    pos = MAXWIDTH/2;
    
    // Create and Join Thread
    int tid;
    pthread_t userControl;
    pthread_t draw;
    pthread_create(&userControl, NULL, getInput, NULL);
    pthread_create(&draw, NULL, drawArea, NULL);
    pthread_join(userControl, NULL);
    pthread_join(draw, NULL);
    return 0;
}
