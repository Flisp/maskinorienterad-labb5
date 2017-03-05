#ifndef GAMEOBJ_H
#define GAMEOBJ_H

#include "gpio.h"

/* En pixel i en spelfigur */
typedef struct tPoint
{
    uint8_t x;
    uint8_t y;
} POINT;

#define MAX_POINTS 20

typedef struct tGeometry
{
    uint8_t numpoints;
    uint8_t sizex;
    uint8_t sizey;
    POINT px[MAX_POINTS];
} GEOMETRY, *PGEOMETRY;

typedef struct tObject
{
    PGEOMETRY geo;
    sint8_t dirx, diry;
    uint8_t posx, posy;
    void (*draw)(struct tObject *);
    void (*clear)(struct tObject *);
    void (*move)(struct tObject *);
    void (*set_speed)(struct tObject *, sint8_t, sint8_t);
} OBJECT, *POBJECT;

void draw_object(POBJECT obj); 
void clear_object(POBJECT obj);
void move_object(POBJECT obj); 
void set_object_speed(POBJECT obj, sint32_t speed_x, sint32_t speed_y); 

#endif 