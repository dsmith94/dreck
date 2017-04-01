/*
 * =====================================================================================
 *
 *       Filename:  block.c
 *
 *    Description:  Text block subsystem
 *
 *        Version:  1.0
 *        Created:  03/29/2017 12:33:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Donald J Smith (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>

#include "token.h"
#include "line.h"
#include "block.h"


struct BLOCK *make_empty_block(void)
{

    /* create new block struct, ready to be filled with text */
    struct BLOCK *b = calloc(1, sizeof(struct BLOCK));
    struct LINE *l = make_line();
    b->data = NULL;
    b->length = 0;
    append_line_to_block(b, l);
    return b;

}



struct BLOCK *make_block_from_file(const char *path)
{

    /* accept buffer, probably loaded from a file, and
     * load into a new empty block
     */


    struct BLOCK *b = calloc(1, sizeof(struct BLOCK));
    ALLEGRO_FILE *f = al_fopen(path, "r");
    ALLEGRO_USTR *n = NULL;
    n = al_fget_ustr(f);
    while (n) {
        struct LINE *line = make_line();
        al_ustr_free(line->data);
        al_ustr_remove_chr(n, al_ustr_length(n) - 1);
        line->data = n;
        append_line_to_block(b, line);
        n = al_fget_ustr(f);
    }
    al_fclose(f);
    return b;

}


static void set_block_focus(struct BLOCK *b, const unsigned int position, const unsigned int index)
{

    /* when cursor changes, change block focus too */
    b->focus_line = index;
    b->focus_cursor = position;

}


static bool move_cursor_right(struct BLOCK *block)
{

    /* move cursors right */
    unsigned int a, b;
    for (a = block->length; a--; ) {
        struct LINE *line = block->data[a];
        unsigned int length = al_ustr_length(line->data);
        struct CURSOR *cursor = &line->cursor;
        for (b = 0; b < cursor->count; ++b) {
            if (cursor->point[b] < length) {
                cursor->point[b] += 1;
                set_block_focus(block, cursor->point[b], a);
            } else {
                return false;
            }
        }
    }
    return true;

}



static bool move_cursor_left(struct BLOCK *block)
{

    /* move cursors left */
    unsigned int a, b;
    for (a = block->length; a--; ) {
        struct LINE *line = block->data[a];
        struct CURSOR *cursor = &line->cursor;
        for (b = cursor->count; b--; ) {
            if (cursor->point[b] > 0) {
                cursor->point[b] -= 1;
                set_block_focus(block, cursor->point[b], a);
            } else {
                return false;
            }
        }
    }
    return true;

}


static bool move_cursor_down(struct BLOCK *block)
{

    /* instructions for moving cursors down */
    unsigned int a, b;
    for (a = block->length; a--; ) {
        struct LINE *line = block->data[a];
        struct CURSOR *cursor = &line->cursor;
        if (a == block->length - 1 && cursor->count)
            return false;
        for (b = cursor->count; b--; ) {
            unsigned int cp = cursor->point[b];
            unsigned int length = al_ustr_length(block->data[a + 1]->data);
            if (cp >= length) {
                if (length > 0)
                    cp = length;
                else
                    cp = 0;
            }
            add_cursor_point(&block->data[a + 1]->cursor, cp);
            set_block_focus(block, block->data[a + 1]->cursor.point[block->data[a + 1]->cursor.count - 1], a);
            remove_point(cursor, cursor->point[b]);
        }
    }
    return true;

}



static bool move_cursor_up(struct BLOCK *block)
{

    /* instructions for moving cursor up */
    unsigned int a, b;
    for (a = 0; a < block->length; a++) {
        struct LINE *line = block->data[a];
        struct CURSOR *cursor = &line->cursor;
        if (a == 0 && cursor->count)
            return false;
        for (b = cursor->count; b--; ) {
            unsigned int cp = cursor->point[b];
            unsigned int length = al_ustr_length(block->data[a - 1]->data);
            if (cp >= length) {
                if (length > 0)
                    cp = length;
                else
                    cp = 0;
            }
            add_cursor_point(&block->data[a - 1]->cursor, cp);
            set_block_focus(block, block->data[a - 1]->cursor.point[block->data[a - 1]->cursor.count - 1], a);
            remove_point(cursor, cursor->point[b]);
        }
    }
    return true;

}



bool move_cursor_command(struct BLOCK *block, struct TOKEN t)
{

    /* move cursor given direction, number of steps */
    char direction = t.direction;
    if (!verify_token_direction(direction))
        return false;
    switch (direction) {
        case DIR_UP:
            return move_cursor_up(block);
        case DIR_DOWN:
            return move_cursor_down(block);
        case DIR_LEFT:
            return move_cursor_left(block);
        case DIR_RIGHT:
            return move_cursor_right(block);
    }
    return false;

}



void handle_block_insertion(struct BLOCK *b, ALLEGRO_USTR *chars)
{

    /*  block insertion managment
     *  called when we have new characters to enter
     */
    int i;
    for (i = b->length; i--; ) {
        handle_line_insertion(b->data[i], chars);
    }

}


void insert_line_in_block(struct BLOCK *b, struct LINE *l, const unsigned int index)
{

    /* put line into block */
    unsigned int i = 0;
    b->data = realloc(b->data, sizeof(struct LINE *) * (b->length + 1));
    for (i = b->length; i > index; i--) {
        b->data[i] = b->data[i - 1];
        b->data[i]->number += 1;
    }
    b->length += 1;
    l->number = index;
    b->data[index] = l;

}


void append_line_to_block(struct BLOCK *b, struct LINE *l)
{

    /* put line at end of block */
    insert_line_in_block(b, l, b->length);

}


void split_line_in_block(struct BLOCK *block, struct LINE *a, const unsigned int index, const bool refresh_cursor)
{

    /* split line, creating new one, and adjusting line
     * numbers 
     */
    struct LINE *b = split_line(a, index, refresh_cursor);
    insert_line_in_block(block, b, b->number);

}


bool remove_line_from_block(struct BLOCK *block, const unsigned int a, const bool make_new_cursor)
{

    /* remove lines from block, in quantity of (steps)
     * if called to do so, add a new cursor in the next line up 
     */
    unsigned int b;
    if (a > 0) {
        combine_line(block->data[a - 1], al_ustr_length(block->data[a - 1]->data), block->data[a]);
        if (make_new_cursor)
            add_cursor_point(&block->data[a - 1]->cursor, al_ustr_length(block->data[a - 1]->data));
    } else {
        return false;
    }
    for (b = a; b < (block->length - 1); b++)
        block->data[b] = block->data[b + 1];
    block->length--;
    block->data = (struct LINE **)realloc(block->data, sizeof(struct LINE **) * block->length);
    return false;

}


void make_newline_in_block(struct BLOCK *block, const char direction, const unsigned int steps)
{

    /* find active cursors and split lines */
    int a, b, c;
    bool refresh_cursor = true;
    if (direction == DIR_UP)
        refresh_cursor = false;
    for (a = steps; a--; ) {
        for (b = block->length; b--; ) {
            struct LINE *line = block->data[b];
            for (c = line->cursor.count; c--;) {
                split_line_in_block(block, line, line->cursor.point[c], refresh_cursor);
            }
        }
    }

}


bool remove_char_from_block(struct BLOCK *block, unsigned int direction, unsigned int steps)
{

    /* remove characters in block -- left side is the left, any other is right
     * side -- and in all cursors
     */
    unsigned int a, b, c;
    bool v = true;
    for (a = steps; a--; ) {
        for (b = block->length; b--; ) {
            struct LINE *line = block->data[b];
            for (c = line->cursor.count; c--;) {
                if (!al_ustr_length(line->data)) {
                    v = remove_line_from_block(block, b, true);
                } else {
                    if (direction == DIR_RIGHT) {
                        v = delete_char_in_line(line, line->cursor.point[c] - 1);
                        if (line->cursor.point[c] > 0)
                            line->cursor.point[c] -= 1;
                    } else {
                        if (line->cursor.point[c] == al_ustr_length(line->data)) {
                            if (b < block->length - 1)
                                v = remove_line_from_block(block, b + 1, false);
                        } else {
                            v = delete_char_in_line(line, line->cursor.point[c]);
                        }
                    }
                }
                if (!v)
                    return false;
            }
        }
    }
    return false;

}


void handle_block_camera(struct BLOCK *b, const unsigned int height)
{

    /* keep block in view at all times */
    /*  
    if (b->focus_line > b->camera_y + (height - 8))
        b->camera_y++;
    if (b->camera_y + height > b->length)
        b->camera_y - height;
    */
    if (b->focus_line > b->camera_y + (height - 5))
        b->camera_y = b->focus_line - (height - 6);
    else if (b->focus_line < b->camera_y + 5)
        b->camera_y = b->focus_line + 6;
    if (b->camera_y < 0)
        b->camera_y = 0;

}


void kill_block(struct BLOCK *b)
{

    /* terminate block in memory */
    unsigned int i;
    if (b) {
        for (i = 0; i < b->length; ++i)
            kill_line(b->data[i]);
        free(b->data);
        free(b);
    }

}



