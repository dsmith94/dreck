#ifndef SEARCH_H
#define SEARCH_H

#include <allegro5/allegro.h>


struct MARK {

    unsigned int start;
    unsigned int end;

};

/* common search result data */
struct SEARCH {

    struct MARK *match;
    unsigned int count;

};


struct SEARCH *make_search(const ALLEGRO_USTR *needle, const ALLEGRO_USTR *haystack);
void kill_search(struct SEARCH *s);


#endif
