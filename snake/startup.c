/*
 * 	startup.c
 *
 * Port D 8-15 : Keypad
 * Port E      : Display
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "gpio.h"
#include "ascii.h"
#include "graphics.h"
#include "timer.h"
#include "keypad.h"

void startup(void) __attribute__((naked)) __attribute__((section (".start_section")) );

void startup ( void )
{
    asm volatile(
    " LDR R0,=stack_top\n"	/* set stack */
    " MOV SP,R0\n"
    " BL  crt_init\n"		/* init C-runtime library */
    " BL  main\n"			/* call main */
    ".L1: B .L1\n"			/* never return */
    ) ;
}

#define INITIAL_LENGTH 4
#define INITIAL_DELAY 300
#define SPEEDUP_PER_POINT 2
/* INITIAL_DELAY minst 100 * SPEEDUP_PER_POINT */

#define SNAKE_RAND_MAX 32767

/*länkad lista*/
typedef struct segment {
    uint8_t x;
    uint8_t y;
    struct segment *next;
} SEGMENT;

/*globala variabler*/
static volatile int score = 0;
static SEGMENT *tail;
static SEGMENT *head;
static sint32_t dirx, diry;
static uint8_t foodx, foody;
static uint32_t loop_count = 0;
static uint32_t next = 1;


static uint32_t snake_rand(void)
{
    next = next * 1103515245 + 12345;
    return (uint32_t)(next/65536) % 32768;	
}

/*starta slupmgenerator*/
static void snake_srand(uint32_t seed)
{
    next = seed;
}

/*generellt- rita ut 4 pixlar*/
static void draw_object(uint8_t x, uint8_t y, uint8_t set)
{
    pixel(x * 2    , y * 2    , set);
    pixel(x * 2 + 1, y * 2    , set);
    pixel(x * 2    , y * 2 + 1, set);
    pixel(x * 2 + 1, y * 2 + 1, set);
}

static void draw_segment(SEGMENT *segment, uint8_t set)
{
    draw_object(segment->x, segment->y, set); 
}

void show_main_menu(void)
{
    /* Töm display, skriv ut text */
    ascii_init();
    ascii_gotoxy(1, 1);  /*sätt markören*/
    ascii_print("Press A to start");
    
    /* Vänta på att knappen A trycks */
    while (keyb() != 0x0A) 
        loop_count++;  /*skapa ett slumptal*/
    
    /* Tömmer display igen */
    ascii_init();
}

/*flytta ett svans-segment så det blir det nya huvudet*/ 
static void move_snake(void)
{
    SEGMENT *move = tail;
    
    tail = move->next;           /*ny svans*/
    move->x = head->x + dirx;    /*ändra koordinaterna så att den blr huvudet*/
    move->y = head->y + diry;
    head->next = move;          /*peka på head*/
    head = move;
    head->next = NULL;          /*head pekar på NULL- sista blocket*/   
}

/*kolla om maten hamnar inuti ormens kropp*/
static int is_food_in_snake(void)
{
    SEGMENT *ptr; 
    ptr=tail; 
    while(ptr!=NULL)
    {
        if(ptr->x==foodx && ptr->y== foody)
            return 1;
        ptr = ptr->next; 
    }
    return 0; 
}

static void create_food(void)
{
    do{
        foodx = snake_rand() % 32 + 16;  /*maten hamnar långt från kanten*/
        foody = snake_rand() % 16 + 8;
    }
    while(is_food_in_snake());
    draw_object(foodx, foody, 1);
}

/*lägg till ett segment*/
static void grow_snake(void)
{
    SEGMENT *new_tail;  
    new_tail=malloc(sizeof(SEGMENT)); 
    new_tail->next = tail;
    new_tail ->x = tail->x;    /*samma position som tail, pgv svårt att veta var svansen är på väg*/
    new_tail ->y = tail->y;
    tail = new_tail;     
}

static void init_game(void)
{
    int i, x, y;
    
    graphic_clearScreen();
    
    /*Rita ramen*/
    for (x = 1; x <= 126; x++)
        pixel(x, 62, 1);
    for (x = 1; x <= 126; x++)
        pixel(x, 1, 1);
    for (y = 1; y <= 62; y++)
        pixel(1, y, 1);
    for (y = 1; y <= 62; y++)
        pixel(126, y, 1);
    
    tail = malloc(sizeof(SEGMENT));     /*reservera plats på heapen för ett segment*/
    head = tail;
    tail->x = 30;                       /*placera den första segmenten i mitten på skärmen*/
    tail->y = 15;
    head->next = NULL;
    
    for (i = 1; i < INITIAL_LENGTH; i++) {  
        grow_snake(); 
    }
    
    dirx = 1;
    diry = 0;
    
    snake_srand(loop_count);                  /*starta slumpgenerator*/
    create_food(); 
    ascii_gotoxy(1, 1);
    ascii_print("Score: 0");
    score = 0;
}

static int is_food_eaten(void)
{
    if (head->x == foodx && head->y == foody)
        return 1; 
    else
        return 0; 
}

static int is_snake_at_border(void)
{
    if (head->x == 0 || head->x == 63)
        return 1;
    else if (head->y == 0 || head->y == 31)
        return 1;
    else
        return 0;
}

static int does_snake_eat_itself(void)
{
    SEGMENT *ptr; 
    ptr = tail;
    
    while(ptr != head)
    {
        if(ptr->x == head->x && ptr->y == head->y)
            return 1;
        ptr = ptr->next; 
    }
    return 0; 
}

static void control_snake(void)
{
    uint8_t c = keyb();
    switch(c) {
    case 6:
        dirx = 1;
        diry = 0;
        break;
    case 4:
        dirx = -1;
        diry = 0;
        break;
    case 2:
        dirx = 0;
        diry = -1;
        break;
    case 8:
        dirx = 0;
        diry = 1;
        break;
    }
}

/* returnerar TRUE (1) om poäng är jämnt delbar med tio */
static int print_score(void)
{
    uint8_t ascii;
    int local_score = score;
    
    if (local_score > 99)
        local_score = 99;
    
    /* tiotalen */
    if (local_score >= 10) {
        ascii = '0';
        
        while (local_score >= 10) {
            ascii++;
            local_score = local_score - 10;
        }
        
        ascii_write_char(ascii);
    }
    
    /* ettalet */
    ascii_write_char(local_score + '0');
    if (local_score == 0)
        return 1;
    else
        return 0;
}

static void full_print_score(void) {
    ascii_gotoxy(1, 1);
    ascii_print("Score: ");
    
    if (print_score()) {
        ascii_gotoxy(1, 2);
        ascii_print("Good job!");
    }
    else {
        ascii_gotoxy(1, 2);
        ascii_print("         ");
    }
}

static void free_snake(void)
{
    SEGMENT *ptr = tail;
    SEGMENT *next;
    
    while (ptr != NULL) {
        next = ptr->next;
        free(ptr);
        ptr = next;
    }
    
    head = NULL;
    tail = NULL;
}

void play_game(void)
{
    int gameover = 0;   /*inte är game over just nu*/
    init_game();
    
    while (!gameover) {
        control_snake();          /*läs från tangentbord och uppdatera ormens riktning*/
        draw_segment(tail, 0);    /*radera pixlar först*/
        move_snake();             /*beräkna var det nya huvudet ska vara*/
        
        if (is_snake_at_border())
            gameover = 1;
        else if (does_snake_eat_itself())
            gameover = 1;
        else if (is_food_eaten()) {
            score++;
            if (score == 99)
                gameover = 1;
            grow_snake();
            grow_snake();
            create_food();
            full_print_score();
        }
        
        draw_segment(head, 1);      /*rita ut det nya huvudet*/
        
        delay_ms(INITIAL_DELAY - (score * SPEEDUP_PER_POINT));              /*kolla hur långsamt den är*/
    }
    
    free_snake(); 
}

void show_game_over(void)
{
    /* Tömmer display, skriver ut text */
    ascii_init();
    ascii_gotoxy(1, 1);
    if (score == 99)
        ascii_print("YOU'RE THE WINNER!");
    else
        ascii_print("GAME OVER, LOSER!");
    ascii_gotoxy(1, 2);
    ascii_print("Your score: ");
    print_score();
    
    /* Väntar två sekunder */
    delay_ms(2000);
    
    /* Tömmer display igen */
    ascii_init();
}


void main(void)
{
    keypad_init();   
    graphic_init();
    
    while (1) {
        show_main_menu();
        play_game();
        show_game_over();
    }
}

