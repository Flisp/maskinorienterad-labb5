#ifndef TYPES_H
#define TYPES_H

/*alias skapas för att undvika onödiga repetitioner*/

#ifndef _UINT32_T_DECLARED
#ifndef __int32_t_defined
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
#endif
#endif

typedef signed   int   sint32_t;
typedef signed   short sint16_t;
typedef signed   char  sint8_t;

#endif