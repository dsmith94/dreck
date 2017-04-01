/*
 * =====================================================================================
 *
 *       Filename:  search.c
 *
 *    Description:  search string/line/block functions
 *
 *        Version:  1.0
 *        Created:  03/29/2017 12:07:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Donald J Smith (dsmith94@gmail.com), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include <stdlib.h>
#include <allegro5/allegro.h>

#include "search.h"


struct MARK make_mark(const unsigned int start, const unsigned int end)
{

    /* make new mark at start -- end code points */
    struct MARK m;
    m.start = start;
    m.end = end;
    return m;

}


void kill_mark(struct MARK *m)
{

    /* remove mark from memory */
    free(m);

}


struct SEARCH *make_search(const ALLEGRO_USTR *needle, const ALLEGRO_USTR *haystack)
{

    /* create new search struct */
    int code_point = 0;
    size_t length = al_ustr_length(needle);
    struct SEARCH *s = calloc(1, sizeof(struct SEARCH));
    while (code_point != -1) {
        code_point = al_ustr_find_set(haystack, code_point, needle);
        if (code_point != -1) {
            struct MARK m = make_mark(code_point, length);
            s->match = realloc(s->match, sizeof(struct MARK) * (s->count + 1));
            s->match[s->count] = m;
            s->count++;
        }
    }
    return s;

}


void kill_search(struct SEARCH *s)
{

    /* terminate search from memory */
    if (s) {
        free(s->match);
        free(s);
    }

}


