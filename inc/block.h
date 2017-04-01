#ifndef BLOCK_H
#define BLOCK_H

#include <allegro5/allegro.h>

struct LINE;
struct TOKEN;

struct BLOCK {
    
    /* data inside block, line-by-line */
    struct LINE **data;

    /* index of most recent, active line */
    unsigned int focus_line;

    /* index of most recent, active cursor */
    unsigned int focus_cursor;

    /* index of camera, for gui display */
    unsigned int camera_y;

    /* length used for block */
    unsigned int length;

};



struct BLOCK *make_empty_block(void);
struct BLOCK *make_block_from_file(const char *path);
void kill_block(struct BLOCK *b);
void handle_block_insertion(struct BLOCK *b, ALLEGRO_USTR *chars);
void append_line_to_block(struct BLOCK *b, struct LINE *l);
void split_line_in_block(struct BLOCK *block, struct LINE *a, const unsigned int index, const bool refresh_cursor);
void insert_line_in_block(struct BLOCK *b, struct LINE *l, const unsigned int index);
bool move_cursor_command(struct BLOCK *block, struct TOKEN t);
bool remove_char_from_block(struct BLOCK *block, unsigned int direction, unsigned int steps);
bool remove_line_from_block(struct BLOCK *block, const unsigned int a, const bool make_new_cursor);
void make_newline_in_block(struct BLOCK *block, const char direction, const unsigned int steps);
void handle_block_camera(struct BLOCK *b, const unsigned int height);


#endif
