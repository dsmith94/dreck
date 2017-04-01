#ifndef LINE_H
#define LINE_H

#include <allegro5/allegro.h>
#include "cursor.h"

struct LINE {

    /* data for line, in unicode utf-8 */
    ALLEGRO_USTR *data;

    /* line cursor system */
    struct CURSOR cursor;

    /* line number */
    unsigned int number;

};



struct LINE *make_line(void);
struct LINE *make_line_from_string(ALLEGRO_USTR *s);
void combine_line(struct LINE *dst, unsigned int index, struct LINE *src);
void handle_line_insertion(struct LINE *l, const ALLEGRO_USTR *chars);
void append_line(struct LINE *dst, struct LINE *src);
bool delete_char_in_line(struct LINE *l, unsigned int index);
struct LINE *split_line(struct LINE *a, const unsigned int index, const bool refresh_cursor);
void kill_line(struct LINE *l);


#endif

