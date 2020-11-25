#include "header.h"

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

void *getInput(void *vargp)
{
    clearScreen();
    printf("Start\n");
    char c;
    while(PLAYING)
    {
        printf("\e[?25l");
        input = getch();
        move();
        // printf("\nTyped:%c | %d\n", input, input);
        // fflush(stdout);
        if(input == 'q') break;
    }
    pthread_exit(NULL);
}

void move()
{
    if(((input == K_LEFT) || (input == ARROW_LEFT)) && (pos != 1)) pos--;
    if(((input == K_RIGHT) || (input == ARROW_RIGHT)) && (pos != MAXWIDTH-2)) pos++;
}

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
                    printf("+");
                    fflush(stdout);
                }
                else if(i == 0 || i == MAXHEIGHT-1)
                {
                    printf("-");
                    fflush(stdout);
                }
                else if((i > 0 && i < MAXHEIGHT-1) && (j == 0 || j == MAXWIDTH-1))
                {
                    printf("|");
                    fflush(stdout);
                }
                else
                {
                    if(i == MAXHEIGHT-2 && (j == pos))
                    {
                        CYAN(0);
                        printf("A");
                        RESETCOLOR();
                    }
                    else printf(" ");
                    fflush(stdout);
                }
            }
            printf("\n");
            fflush(stdout);
        }
        if(input == 'q')break;
        msleep(100);
    }
    pthread_exit(NULL);
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