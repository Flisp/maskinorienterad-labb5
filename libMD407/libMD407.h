/*libMD407.h* Declaration of library functions, constants etc...*/
 
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

/* Type definitions */
/*typedef struct tag_usart
{
    volatile unsigned short sr;
    etc...
	
*/
/* Constants */
/*#define USART1 ((USART*)0x40011000)*/

/* Constant defined by linker */
extern char heap_low;
extern char heap_top;
extern char _sbss;
extern char _ebss;

/* Library defined functions */
void _outchar(char c);
char _tstchar(void);
char _inchar(void);
void crt_init(void);
char* _sbrk(int);
int _close(int file);
int _fstat(int file, struct stat* st);
int _isatty(int file); 
int _open(const char* name, int flags, int mode);
int _lseek(int file, int ptr, int dir);
int _write(int file, char* ptr, int len);
int _read (int file, char* ptr,int len); 
