/*
	Default linker script for MD407 (STM32F407)
	All code and data goes to RAM.
*/

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
  RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 108K
}

SECTIONS
{
     .text :
    {
	. = ALIGN(4);	    
	*(.start_section)	/* startup code */
	*(.text) 			/* remaining code */
        *(.text.*)  
	_sbss = .;
	*(.bss)			/* unintialised data */
        *(COMMON)
	_ebss = .;
	*(.data)			/* initialised data */
        *(.data.*)
	*(.rodata)  		/* read-only data (constants) */
        *(.rodata.*)
	. = ALIGN(4);
    } >RAM
    . = ALIGN(256);
	heap_low = .;
	. = . + 0x1000;			/* 4 kbyte heap */
	heap_top = .;
	. = . + 0x400;			/* 1 kbyte stack */
	stack_top = .;
 }
