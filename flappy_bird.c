#include <curses.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#define CHAR_BIRD 'O' //Define the bird character
#define CHAR_STONE '*' //Define the stoens that make up the pillars
#define CHAR_BLANK ' '//Define the empty character

typedef struct node{
    int x, y;
    struct node *next;
}node, *Node;

Node head, tail;
int bird_x, bird_y;
int ticker;

void init();   // Initialization function that manages the initialization tasks of the game
void init_bird(); // Initialize the bird's position coordinates
void init_draw();// Initialize the background
void init_head();  // Initialize the linked list head that stores the pillars
void init_wall();  // Initialize the linked list that stores the pillars
void drop(int sig);// Signal reception function to receive system signals and move the pillars from right to left
int set_ticker(int n_msec);// Set the kernel's timer tick interval

int set_ticker(int n_msec)
{
    struct itimerval timeset;
    long n_sec, n_usec;
    
    n_sec = n_msec / 1000;
    n_usec = (n_msec % 1000) * 1000L;

    timeset.it_interval.tv_sec = n_sec;
    timeset.it_interval.tv_usec = n_usec;

    timeset.it_value.tv_sec = n_sec;
    timeset.it_value.tv_usec = n_usec;

    return setitimer(ITIMER_REAL, &timeset, NULL);
    
}

void drop(int sig)
{
    int j;
    Node tmp, p;

    //clear the symbol at the original bird position
    move(bird_y, bird_x);
    addch(CHAR_BLANK);
    refresh();

    //Update the position of the bird and refresh the screen
    bird_y++;
    move(bird_y, bird_x);
    addch(CHAR_BIRD);
    refresh();

    //End the game if the bird collides the pillar
    if((char)inch() == CHAR_STONE)
    {
        set_ticker(0);
        sleep(1);
        endwin();
        exit(0);
    }

    //Check if the first wall goes beyond the boudary
    p = head->next;
    if(p-> x<0)
    {
        head->next = p->next;
        free(p);
        tmp = (node *)malloc(sizeof(node));
        tmp-> x = 99;
        tmp->y = rand() % 11 + 5;
        tail->next = tmp;
        tmp->next = NULL;
        tail = tmp;
        ticker -=10; 
        set_ticker(ticker);
    }
    //draw a new pillar
    for(p = head->next; p->next !=NULL; p->x--, p = p->next)
    {
        //replace the charstone with charblank
        for(j = 0; j < p->y; j++)
        {
            move(j, p->x);
            addch(CHAR_BLANK);
            refresh();
        }
        for(j = p->y+5; j<=23; j++)
        {
            move(j,p->x);
            addch(CHAR_BLANK);
            refresh();
        }

        if(p->x-10 >= 0 && p->x < 80)
        {
            for(j = 0; j < p->y; j++)
            {
                move(j, p->x-10);
                addch(CHAR_STONE);
                refresh();
            }
            for(j = p->y + 5; j<= 20; j++)
            {
                move(j, p->x-10);
                addch(CHAR_STONE);
                refresh();
            }
        }

    }
    tail->x--;
}

int main()
{
    char ch;

    init();
    while(1)
    {
        ch = getch(); //get keyboard input
        if(ch == ' ' || ch== 'w' || ch =='W') //If spacebar or "w" key is pressed
        {
            //move the bird and redraw it
            move(bird_y, bird_x);
            addch(CHAR_BLANK);
            refresh();
            bird_y--;
            move(bird_y, bird_x);
            addch(CHAR_BIRD);
            refresh();

            //if the bird hits the pipes, end the game
            if((char)inch() == CHAR_STONE)
            {
                set_ticker(0);
                sleep(1);
                endwin();
                exit(0);
            }
        }

        else if(ch == 'z' || ch == 'Z') //pause
        {
            set_ticker(0);
            do
            {
                ch = getch();
            }while(ch != 'z' && ch != 'Z');
            set_ticker(ticker);
        }
        else if(ch=='q' || ch == 'Q')// quit
        {
            sleep(1);
            endwin();
            exit(0);
        }
    }
    return 0 ;
}

void init()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    srand(time(0));
    signal(SIGALRM, drop);

    init_bird();
    init_head();
    init_wall();
    init_draw();
    sleep(1);
    ticker = 500;
    set_ticker(ticker);
}

void init_bird()
{
    bird_x = 5;
    bird_y = 15;
    move(bird_y, bird_x);
    addch(CHAR_BIRD);
    refresh();
    sleep(1);
}

void init_head()
{
    Node tmp;

    tmp = (node *)malloc(sizeof(node));
    tmp ->next =NULL;
    head = tmp;
    tail = head;
}

void init_wall()
{
    int i;
    Node tmp, p;

    p=head;
    for(i = 0; i<5; i++)
    {
        tmp = (node *)malloc(sizeof(node));
        tmp->x =(i+1) * 19;
        tmp->y = rand()%11 + 5;
        p->next = tmp;
        tmp->next = NULL;
        p = tmp;
    }
    tail = p;
}

void init_draw()
{
    Node p;
    int i, j;

    //traverse the linked list
    for(p = head->next; p->next !=NULL; p = p->next)
    {
        //draw the pipes
        for(i = p->x; i> p->x-10; i--)
        {
            for(j=0; j< p->y; j++)
            {
                move(j, i);
                addch(CHAR_STONE);
            }
            for(j = p->y+5; j<=23; j++)
            {
                move(j, i);
                addch(CHAR_STONE);
            }
        }
        refresh();
        sleep(1);
    }
}