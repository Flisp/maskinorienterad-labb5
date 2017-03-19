#include "gpio.h"
#include "graphics.h"
#include "bits.h"
#include "timer.h"

void display_init(void)
{
    RCC_AHB1ENR |= RCC_GPIO_E;   /*aktivera klockan för port E*/
    GPIO_E.moder = 0x55555555;   /*port E utport*/ 
    GPIO_E.otyper = 0x00000000;    /*push pull*/
    GPIO_E.ospeedr = 0x55555555;   /*medium speed, om problem sätt till låg*/
    GPIO_E.pupdr = 0x55550000;    /* pullup inputs*/
}

/* Ettställer bitarna i x */
static void graphic_ctrl_bit_set(uint8_t x)
{
    GPIO_E.odr_low &= ~B_SELECT;    /* Välj GRAFIK-display (B_SELECT = 0) */
    GPIO_E.odr_low |= x;
}

/* Nollställer bitarna i x */
static void graphic_ctrl_bit_clear(uint8_t x)
{
    GPIO_E.odr_low &= ~B_SELECT;    /* Välj GRAFIK-display */
    GPIO_E.odr_low &= ~x; 
}

/* Väljer vilken halva av skärmen som ska adresseras */
static void select_controller (uint8_t controller)
{
    switch (controller)
    {
        case 0:
            graphic_ctrl_bit_clear (B_CS1 | B_CS2); 
            break; 
        case B_CS1 :
            graphic_ctrl_bit_set(B_CS1); graphic_ctrl_bit_clear (B_CS2); 
            break; 
        case B_CS2 :
            graphic_ctrl_bit_set(B_CS2); graphic_ctrl_bit_clear (B_CS1); 
            break;
        case (B_CS1 | B_CS2):
            graphic_ctrl_bit_set(B_CS1 | B_CS2); 
            break;   
    }
}

static void graphic_wait_ready(void)
{
    uint8_t c; 
    
    graphic_ctrl_bit_clear(B_E);    /* enable */
    GPIO_E.moder = 0x00005555;      /* bit 15-08 inputs, 7-0 outputs*/
    graphic_ctrl_bit_clear (B_RS);  /* RS väljer kommando eller data */
    graphic_ctrl_bit_set(B_RW);
    
    delay_500ns();
    
    while (1)
    {
        graphic_ctrl_bit_set(B_E); 
        delay_500ns(); 
        graphic_ctrl_bit_clear (B_E); 
        c = GPIO_E.idr_high & LCD_BUSY;      /* Vänta tills displayen inte är upptagen */
        if (c== 0) break; 
        delay_500ns(); 
    }
    delay_500ns(); 
    graphic_ctrl_bit_set (B_E); 
    GPIO_E.moder = 0x55555555; 
}

static uint8_t display_read(uint8_t controller)
{
    uint8_t c; 
    GPIO_E.moder = 0x00005555;      /* Sätter pinnar 15-8 som in, 7-0 som ut */
    /*graphic_ctrl_bit_clear(B_E);*/
    graphic_ctrl_bit_set (B_RS | B_RW); /* B_RS = 1 : Data (inte kommando)
                                           B_RW = 1 : Läs (inte skriv) */
    select_controller(controller);
    delay_500ns();
    graphic_ctrl_bit_set(B_E);
    delay_500ns();
    
    c = GPIO_E.idr_high;                /* Läs data från displayens minne */
    
    graphic_ctrl_bit_clear (B_E);       
    GPIO_E.moder = 0x55555555;
    
    if(controller & B_CS1) {            /* Om biten B_CS1 är satt: */
        select_controller (B_CS1);      /* Välj controller B_CS1 */
        graphic_wait_ready(); 
    }
    if (controller & B_CS2) {
        select_controller(B_CS2);
        graphic_wait_ready(); 
    }
    return c; 
}

/* Display-minnet måste läsas två gånger! */
static uint8_t graphic_read(uint8_t controller)
{
    display_read(controller);
    return display_read(controller); 
}

static void graphic_write(uint8_t val, uint8_t controller)
{
    GPIO_E.odr_high  = val; 
    select_controller (controller);
    delay_500ns(); 
    graphic_ctrl_bit_set(B_E); 
    delay_500ns(); 
    graphic_ctrl_bit_clear(B_E); 
   
    if( controller & B_CS1) {
        select_controller (B_CS1);
        graphic_wait_ready(); 
    }
    if (controller & B_CS2) {
        select_controller(B_CS2);
        graphic_wait_ready(); 
    }
    
    GPIO_E.odr_high = 0; 
    graphic_ctrl_bit_set (B_E); 
    select_controller (0); 
}

static void graphic_writeCommand(uint8_t cmd, uint8_t controller)
{
    
    graphic_ctrl_bit_clear (B_E);
    graphic_ctrl_bit_clear (B_DI | B_RW); 
    graphic_write (cmd, controller); 
}

static void graphic_writeData(uint8_t data, uint8_t controller)
{
   
    graphic_ctrl_bit_clear (B_E); 
    graphic_ctrl_bit_set(B_DI); 
    graphic_ctrl_bit_clear (B_RW); 
    graphic_write (data, controller); 
}

void graphic_init(void)
{
	display_init();
	
    graphic_ctrl_bit_set (B_E); 
    delay_micro(10);
    graphic_ctrl_bit_clear (B_CS1 | B_CS2 | B_RST | B_E);
    delay_ms(30);
    
    graphic_ctrl_bit_set (B_RST);
    
    graphic_writeCommand (LCD_OFF, B_CS1|B_CS2);            /* toggla display */
    graphic_writeCommand (LCD_ON, B_CS1|B_CS2);             
    graphic_writeCommand (LCD_DISP_START, B_CS1|B_CS2);     /* start = 0 */
    graphic_writeCommand (LCD_SET_ADD, B_CS1|B_CS2);        /* add = 0 */
    graphic_writeCommand (LCD_SET_PAGE, B_CS1|B_CS2);       /* page = 0 */
    
    select_controller (0); 
}

void graphic_clearScreen(void)
{
    uint8_t i, j;
    
    for(j = 0; j < 8; j++) {
        graphic_writeCommand(LCD_SET_PAGE | j, B_CS1|B_CS2 );
        graphic_writeCommand(LCD_SET_ADD | 0,  B_CS1|B_CS2 );
		for(i = 0; i <= 63; i++){
			graphic_writeData(0, B_CS1|B_CS2);
		}
    }
}

void pixel( uint32_t x, uint32_t y, uint32_t set)
{
    uint8_t mask, c, controller;
    uint32_t index;
	
    if( (x > 127 ) || (y > 63) )
        return;
		
    index = y / 8;
	mask = 1 << (y % 8);
    
    if(x > 63){
		controller = B_CS2;
		x = x - 64;
    }
    else {
		controller = B_CS1;
    }
	
    graphic_writeCommand(LCD_SET_ADD | x, controller );
    graphic_writeCommand(LCD_SET_PAGE | index, controller );
    c = graphic_read (controller);
    graphic_writeCommand(LCD_SET_ADD | x, controller );
    
    if(set)
		mask = c | mask;
    else 
		mask = c & ~mask;
    
    graphic_writeData(mask, controller);
    graphic_writeCommand (LCD_ON, B_CS1|B_CS2);
}
      

