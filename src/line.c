/*
 * =====================================================================================
 *
 *       Filename:  line.c
 *
 *    Description:  Handle line functions in ked
 *
 *        Version:  1.0
 *        Created:  03/28/2017 10:32:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Donald J Smith (dsmith94@gmail.com)
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cursor.h"
#include "line.h"
#include "search.h"


struct LINE *make_line(void)
{

    /* create new line and return it */
    struct LINE *l = (struct LINE *)calloc(1, sizeof(struct LINE));
    l->number = 0;
    l->data = al_ustr_new("");
    l->cursor = make_cursor();
    return l;

}



struct LINE *make_line_from_string(ALLEGRO_USTR *s)
{

    /* create new line and return it */
    struct LINE *l = make_line();
    al_ustr_free(l->data);
    l->data = s;
    return l;

}


void combine_line(struct LINE *dst, unsigned int index, struct LINE *src)
{

    /* insert src into dst at index */
    al_ustr_insert(dst->data, index, src->data);

}


void append_line(struct LINE *dst, struct LINE *src)
{

    /* put src onto end of dst */
    combine_line(dst, al_ustr_length(dst->data) - 1, src);

}


void handle_line_insertion(struct LINE *l, const ALLEGRO_USTR *chars)
{

    /* if this line has an active insertion point, add 
     * unicode chars in
     */
    int i;
    for (i = l->cursor.count; i--; ) {
        al_ustr_insert(l->data, l->cursor.point[i], chars);
        l->cursor.point[i] += al_ustr_length(chars);
    }

}


struct LINE *split_line(struct LINE *a, const unsigned int index, const bool refresh_cursor)
{

    /* split line in two, creating a new one 
     * return new line, old one is cut at
     * index 
     */
    struct LINE *b;
    ALLEGRO_USTR *s = al_ustr_dup_substr(a->data, 
            al_ustr_offset(a->data, index), 
            al_ustr_size(a->data));
    b = make_line();
    al_ustr_free(b->data);
    b->data = s;
    b->number = a->number + 1;
    if (refresh_cursor) {
        reset_cursor(&a->cursor);
        add_cursor_point(&b->cursor, 0);
    }
    al_ustr_truncate(a->data, al_ustr_offset(a->data, index));
    return b;

}


struct SEARCH *search_line(struct LINE *l, ALLEGRO_USTR *search_string)
{

    /* search line for query 
     * return search struct on completion, NULL
     * if none can be found
     */


    struct SEARCH *s = make_search(search_string, l->data);
    return s;

}


bool delete_char_in_line(struct LINE *l, unsigned int index)
{

    /* remove one character from line, at index */
    return al_ustr_remove_chr(l->data, index);

}


void kill_line(struct LINE *l)
{

    /* cleanup line so it doesn't pollute memory */
    if (l) {
        reset_cursor(&l->cursor);
        if (l->data)
            al_ustr_free(l->data);
        free(l);
    }
    l = NULL;

}


