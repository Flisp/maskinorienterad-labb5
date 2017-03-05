
#include "bits.h"

void ascii_ctrl_bit_set(uint8_t bits);
void ascii_ctrl_bit_clear (uint8_t bits);
void ascii_write_controller (uint8_t data); 
void ascii_write_cmd(uint8_t command);
void ascii_write_data(uint8_t data);

uint8_t ascii_read_controller();
uint8_t ascii_read_status();
uint8_t ascii_read_data ();

void ascii_init(void);
void ascii_gotoxy(uint8_t x, uint8_t y);
void ascii_write_char(unsigned char c);  
void ascii_print(char *text);
