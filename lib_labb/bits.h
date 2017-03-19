#ifndef BITS_H
#define BITS_H

#define B_E         0x40      	/*Enable*/      
#define B_SELECT    0x04   		/*Ascii or graphic select*/
#define B_RW        0x02       	/*Read write*/
#define B_RS        0x01       
#define B_RST       0x20      	/*Reset*/
#define B_CS2       0x10      	/*Controller select2*/
#define B_CS1       0x08       	/*controller select 1*/
#define B_DI        0x01     	/*data instruction*/

#define LCD_ON           0x3f    /*Display på*/
#define LCD_OFF          0x3e    /*Display av*/
#define LCD_SET_ADD      0x40    /*x adress (0-63)*/
#define LCD_SET_PAGE     0xb8    /*y adress (0-7)*/
#define LCD_DISP_START   0xc0    /*start adress i display minnne*/   
#define LCD_BUSY         0x80    /*läsa ut busy status . R/W skall vara hög*/   

#endif
