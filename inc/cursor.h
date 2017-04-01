#ifndef CURSOR_H
#define CURSOR_H

struct CURSOR {

    unsigned int *point;

    unsigned int count;

};


struct CURSOR make_cursor(void);
void add_cursor_point(struct CURSOR *c, unsigned int index);
void remove_point(struct CURSOR *c, unsigned int index);
void reset_cursor(struct CURSOR *c);

#endif

