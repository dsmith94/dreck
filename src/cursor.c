
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "cursor.h"


struct CURSOR make_cursor(void)
{

    /* create new cursor */
    struct CURSOR c;
    c.point = NULL;
    c.count = 0;
    return c;

}


void add_cursor_point(struct CURSOR *c, unsigned int index)
{

    /* add cursor point to current cursor
     * cancel operation if point already exists
     */

    unsigned int i;
    i = c->count;
    c->count++;
    c->point = realloc(c->point, sizeof(unsigned int) * c->count);
    c->point[i] = index;

}


void remove_point(struct CURSOR *c, unsigned int index)
{

    /* remove point from cursor, if index matches */
    unsigned int a;
    unsigned int b = c->count - 1;
    for (a = c->count; a--; ) {
        if (c->point[a] == index) {
            c->point[a] = c->point[b];
            c->count--;
            c->point = realloc(c->point, sizeof(unsigned int) * c->count);
            break;
        }
    }


}


void reset_cursor(struct CURSOR *c)
{

    /* reset cursor stats */
    if (c->point)
        free(c->point);
    c->point = NULL;
    c->count = 0;

}


