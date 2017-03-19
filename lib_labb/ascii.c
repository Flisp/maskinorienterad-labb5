#include "gpio.h"
#include "ascii.h"
#include "timer.h"
#include "graphics.h"

/*skriv till styrregistret*/
void ascii_ctrl_bit_set(uint8_t bits)
{
    GPIO_E.odr_low |= (B_SELECT | bits);  /*alltid select bit satt*/
}

/*nolställa bitar i styrregister*/
void ascii_ctrl_bit_clear(uint8_t bits)
{
    GPIO_E.odr_low &= (B_SELECT |~bits);  /*nollställ aldrig select bit. Nollställa bitar som skickas in*/ 
}


/*skriv till display, kommando eller data, generellt*/
void ascii_write_controller(uint8_t data)
{
    ascii_ctrl_bit_set(B_E);  /*enable bit, bit6 */
    GPIO_E.odr_high = data; 
    delay_250ns(); 
    ascii_ctrl_bit_clear(B_E);   /*färdig med skrivningen*/
}

/*skriv kommando till display*/
void ascii_write_cmd(uint8_t command)
{
    ascii_ctrl_bit_clear(B_RS | B_RW);  /*nollställ bit 0 och 1*/
    ascii_write_controller(command);    /*info som ska skickas*/
}

void ascii_write_data(uint8_t data)
{
    ascii_ctrl_bit_set(B_RS); 
    ascii_ctrl_bit_clear(B_RW);  
    ascii_write_controller(data);    
}

uint8_t ascii_read_controller()
{
    uint8_t rv; 
    ascii_ctrl_bit_set(B_E);
    delay_250ns();
    delay_250ns();
    rv= GPIO_E.idr_high;   /*läser från data bussen*/
    ascii_ctrl_bit_clear(B_E); 
    return rv; 
}

uint8_t ascii_read_status()
{   
    uint8_t rv;
    GPIO_E.moder &= 0x0000ffff;    /*konfigurerar port E, bit8-15 som ingångar*/
    ascii_ctrl_bit_clear(B_RS);
    ascii_ctrl_bit_set(B_RW); 
    rv = ascii_read_controller(); 
    GPIO_E.moder |= 0x55550000;    /*port E bit 8-15 utgångar*/
    return rv; 
}

uint8_t ascii_read_data()
{   
    uint8_t rv;
    GPIO_E.moder &= 0x0000ffff;    /*konfigurerar port E*/
    ascii_ctrl_bit_set(B_RS);
    ascii_ctrl_bit_set(B_RW); 
    rv = ascii_read_controller(); 
    GPIO_E.moder |= 0x55550000;
    return rv; 
}

/* Kapslat in allt för att skicka ett kommando i en funktion */
void ascii_command (uint8_t command, uint32_t delay)
{
    while((ascii_read_status() & 0x080)== 0x080) ;
   delay_micro(8);
   ascii_write_cmd(command);
   delay_micro(delay);   /*tid att vänta är olika för olika kommando*/
}

void ascii_init()
{
    display_init();
    
    ascii_command(0x38, 50);/*för att ha 2 rader och en viss storlek, måste kommandot function set vara 0011 1000 alltså N=1, F=0*/
    ascii_command(0x0f, 50); /*kommando:display control, D=1,C=1, B=1  --> 0000 1111*/
    ascii_command(0x01, 2000); /*kommando: clear display*/
    ascii_command(0x06, 50); /*Kommando: entry mode set, ID=1, SH= 0  --> 0000 0110*/
} 

void ascii_gotoxy(uint8_t x, uint8_t y)
{
     uint8_t adress= x-1;
      
    /*om raden är 2, sätt adress till adress + 0x40;*/
    if (y ==2) {
        adress= adress + 0x40;  /* Rad 1 börjar på 0x00, rad 2 börjar på 0x40 */
    }
    ascii_write_cmd(0x80 | adress);
}

void ascii_write_char(unsigned char c)
{
    while((ascii_read_status() & 0x080)== 0x080) ;
    delay_micro(8);
    ascii_write_data(c);
    delay_micro(43);
}

void ascii_print(char *text)
{
    int i = 0;
    while (text[i] != '\0') {
        ascii_write_char(text[i]);
        
        i++;
    }
}

void _outchar(char c)
{
	ascii_write_char(c);
}



