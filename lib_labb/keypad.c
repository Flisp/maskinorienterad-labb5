#include "keypad.h"

/*vi konfigurerar port D genom att skriva 0x5500???? till moder.
 * Mest signifikanta bitar är 4 in- och 4 utpinnar kopplade till ett tangentbord (5500)*/
void keypad_init(void)
{
    const uint32_t Kbd_Disp_pinmode = 0x55000000; 
    const uint32_t Kbd_Disp_pupd = 0x00aa0000;    /*inpinnar från kolumnerna pulluppulldow*/
    
    RCC_AHB1ENR |= RCC_GPIO_D;   /*aktivera klockan för port D*/

	GPIO_D.moder &= 0x0000ffff;
    GPIO_D.moder |= Kbd_Disp_pinmode;     /*skrivning*/
	
    GPIO_D.otyper &= 0x00ff;
	
	GPIO_D.pupdr &= 0x0000ffff;
    GPIO_D.pupdr |= Kbd_Disp_pupd;
}

/*den aktiverar en rad, sätt spänning på "kablarna". Row är ett värde som kan vara 1,2,3,4 eller 0*/
static void kbd_activate(uint32_t row)
{
    if (row < 1 || row > 4) return;
    
    uint32_t pattern = (1 << (3 + row));    /* row = 1 : ettställ bit 4 */
    
    GPIO_D.odr_high &= 0x0f;        /* Skriver ingenting till IN-pinnarna! */
    GPIO_D.odr_high |= pattern;
}

/*läsa av om någon knapp på den aktiva raden är nertrykt */
static int kbd_col (void)
{
    uint8_t c = GPIO_D.idr_high & 0x0f;
	
    if (c & 0x08) return 4;
    if (c & 0x04) return 3;
    if (c & 0x02) return 2; 
    if (c & 0x01) return 1; 
	
    return 0; 
}

/*array med tangentkoder (1,2,3,A,4,5,6,B...) som i figuren*/
static const uint8_t Key[]={ 1,2,3,0xa,4,5,6,0xb,7,8,9,0xc,0xe,0,0xf,0xd };

/*huvudfunktionen för tangentbord*/
uint8_t keyb (void)
{
    uint32_t row, col; 
    
    /*loopa igenom raderna. Vi börjar med rad 1*/
    for(row=1; row<= 4; row ++)
    {
        /*Vi skickar in rad i funktionen som ska aktivera den rad*/
        kbd_activate(row); 
        col = kbd_col();
        
        /*kolla kolumn*/
        if (col !=0)
        {
            kbd_activate(0);
            return Key [4*(row-1)+(col-1)];   /*beräkna index för att välja rätt knapp*/
        }
        kbd_activate (0); 
    }
    return 0xff; 
}