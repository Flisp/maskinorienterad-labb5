/*
* libMD407.c
* MD407 library
*/

/* declarations goes to  'libMD407.h' */
#include "libMD407.h"

/* Define variables here */
static char* heap_end;

int _fstat(int file, struct stat* st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    return 1;
}

int _open(const char* name, int flags, int mode)
{
    return -1;
}

int _close(int file)
{
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

int _write(int file, char* ptr, int len)
{
    int todo;
    for(todo = 0; todo < len; todo++) {
		_outchar(*ptr++);
    }
    return len;
}

int _read(int file, char* ptr, int len)
{
	return 0;
/*    int todo;

    if(len == 0)
		return 0;

    for(todo = 1; todo < len; todo++) {
		*ptr++ = _inchar();
    }
    return todo;*/
}

char* _sbrk(int incr)
{
    char* prev_heap_end;

    if(heap_end == 0) {
		heap_end = &heap_low;
    }
    prev_heap_end = heap_end;

    if(heap_end + incr > &heap_top) {
		/* Heap and stack collision */
		errno = ENOMEM;
		return (char*)-1;
    }
    heap_end += incr;
    return (char*)prev_heap_end;
}

void crt_init()
{
    extern char _sbss; /* Defined by linker */
    extern char _ebss; /* Defined by linker */
	char* s;
    s = &_sbss;
	
    while(s < &_ebss)
		*s++ = 0;
		
    s = &heap_low;
    while(s < &heap_top)
		*s++ = 0;
		
    heap_end = 0;
}

