
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include<stdlib.h>



pthread_mutex_t mutex;
int N ;
int turn = 0;
int isGameOn = 1; // Becomes 0 when game is over
int hasXWon = 0;
int hasOWon = 0;

char ** board;

//Function declerations
void * playX(void * a);
void * playO(void * a);
int playGame(char );


// Thread X uses this function to play on board.
void *playX(void * a){
    while(1){
        pthread_mutex_lock(&mutex);
        if(turn != 1)
            pthread_mutex_unlock(&mutex);
       else if(!isGameOn){
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }else{
            hasXWon = playGame('X');
            if(hasXWon || !isGameOn){
                turn = 2;
                isGameOn = 0;
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
                break;
            }else{
                turn = 2;
                pthread_mutex_unlock(&mutex);
            }
        }
    }
}

// Thread O uses this function to play on board.
void *playO(void * a) {
   
    while(1){
        pthread_mutex_lock(&mutex);
        if(turn != 2)
            pthread_mutex_unlock(&mutex);
       else if(!isGameOn){
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }else{
            hasOWon = playGame('O');
            if(hasOWon || !isGameOn){
                turn = 1;
                isGameOn = 0;
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
                break;
            }
            else{
                turn = 1;
                pthread_mutex_unlock(&mutex);

            }
        }
    }
}

int playGame(char sign){

    int hasPlayed = 0;
    int x;
    int y;
    do
    {
        x = rand() % N;
        y = rand() % N;
        if(board[x][y] == ' '){
            board[x][y] = sign;
            hasPlayed = 1;
            printf("Player %c played on: (%d,%d)\n",sign,x,y);
        }
              
    } while (!hasPlayed);

    //check rows 
    int hasWon = 1;

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            if(board[i][j] != sign)
                hasWon = 0;
        }
        if(hasWon)
            return 1;
        hasWon = 1;
    }

    //check columns

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            if(board[j][i] != sign)
                hasWon = 0;

        }
        if(hasWon)
            return 1;
        hasWon = 1;
    }

    //check diagonal left to right

    for(int i = 0; i < N; i++)
    {
        if(board[i][i] != sign)
            hasWon = 0;

    }
    if(hasWon)
        return 1;
    hasWon = 1;
  
    //check diagonal rigt to left

    for(int i = 0; i < N; i++)
    {
        if(board[i][N-i-1] != sign)
            hasWon = 0;

    }
    if(hasWon)
        return 1;
        


    //check fullness
    int isFull = 1;

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            if(board[i][j] == ' ')
                isFull = 0;
        }
    }
    

    if(isFull){
        isGameOn = 0;
    }

    return 0;
    
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    N = atoi(argv[1]); // size of board taken from command line.

    board = (char **)malloc(sizeof(char*) * N);
   // initialize board with empty strings.
    for(int i = 0; i < N; i++)
    {
        board[i] = (char *) malloc(N);
        for(int j = 0; j < N; j++)
        {
            board[i][j] = ' ';
        }
    }


    printf("Board Size : %dx%d\n", N,N);

    pthread_t p1;
    pthread_t p2;

    int rc1 = pthread_create(&p1, NULL, playX, NULL);
    assert(rc1 == 0);
     
    int rc2 = pthread_create(&p2, NULL, playO, NULL) ;
    assert(rc2 == 0);

    turn = 1; // When both thread created successfully, turn 1 makes player X (p1) to play first.
    
    (void) pthread_join(p1, NULL);
    (void) pthread_join(p2, NULL);

    printf("\nGame end\n");

    if(hasXWon)
        printf("Winner is X\n");

    else if(hasOWon)
        printf("Winner is O\n");
    else
        printf("It is a tie\n");


   //prints board
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            printf("[%c]", board[i][j]);
        }
        printf("\n");
    }

    return 0;
}
