#include "gameObj.h"
#include "graphics.h"

static void draw_or_clear_object(POBJECT obj, uint8_t set)
{
    /* Loopa igenom alla pixlar som är delar av objektet
     * rita ut varje pixel på rätt position (pixelns position + objektets position)
     */
    for (int i = 0; i < obj->geo->numpoints; i++)
    {
        POINT px = obj->geo->px[i];
        pixel(obj->posx + px.x, obj->posy + px.y, set); 
    }
}

void draw_object(POBJECT obj)
{
    draw_or_clear_object(obj, 1); 
}

void clear_object(POBJECT obj)
{
    draw_or_clear_object(obj, 0); 
}

void move_object(POBJECT obj)
{
    clear_object(obj);
    
    sint16_t new_posx = obj->posx + obj->dirx;
    sint16_t new_posy = obj->posy + obj->diry;
    
    /* objektet hamnar för långt åt vänster : studsa och se till
     * att objektet inte går utanför skärmen */
    if (new_posx < 1) {
        obj->dirx = - obj->dirx;
        new_posx = 1;
    }
    
    /* objektet hamnar för långt åt höger : studsa och se till
     * att objektet inte går utanför skärmen */
    if (new_posx + obj->geo->sizex > 128) {
        obj->dirx = - obj->dirx;
        new_posx = 128 - obj->geo->sizex;
    }
    
    if (new_posy < 1) {
        obj->diry = - obj->diry;
        new_posy = 1;
    }
    
    if (new_posy + obj->geo->sizey > 64) {
        obj->diry = - obj->diry;
        new_posy = 64 - obj->geo->sizey;
    }
    
    obj->posx = new_posx;
    obj->posy = new_posy;
    
    draw_object(obj);
}

void set_object_speed(POBJECT obj, sint32_t speed_x, sint32_t speed_y)
{
    obj->dirx = speed_x;
    obj->diry = speed_y;
}