/*
 * =====================================================================================
 *
 *       Filename:  tree.c
 *
 *    Description:  Tree support functions
 *
 *        Version:  1.0
 *        Created:  03/29/2017 02:29:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Donald J Smith (dsmith94@gmail.com), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>

#include <allegro5/allegro.h>

#include "line.h"
#include "block.h"
#include "tree.h"



struct BLOCK *active_block(struct TREE *t)
{

    /* return active block, if it exists */
    unsigned char i = t->active_block;
    if (t->block[i])
        return t->block[i];
    return NULL;

}


void manage_tree(struct TREE *t)
{

    /* handle new data as it is received into the editor tree */

    struct BLOCK *active = active_block(t);

    /* check command buffer */
    if (!al_ustr_equal(t->command_string, al_ustr_empty_string())) {

    }

    /* check input buffer -- if we have anything to add, send it
     * to active block 
     */
    if (active) {
        if (!al_ustr_equal(t->input_string, al_ustr_empty_string())) {
            handle_block_insertion(active, t->input_string);
            al_ustr_assign(t->input_string, al_ustr_empty_string());
        }
    }

}


void append_block_to_tree(struct TREE *t, struct BLOCK *b)
{

    /* add block to end of data tree */
    t->block = realloc(t->block, sizeof(struct BLOCK *) * (t->count + 1));
    t->block[t->count] = b;
    t->count++;

}



static int32_t character_conversion(const int32_t keycode)
{

    const int32_t abort_signals[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 31, 127
    };
    const int chars = sizeof(abort_signals) / sizeof(int32_t);
    for (int i = 0; i < chars; ++i)
        if (abort_signals[i] == keycode)
            return 0;
    return keycode;
}


bool insert_char_to_tree(struct TREE *t, const int32_t keycode)
{

    /* give character to tree, if it is valid */
    if (!character_conversion(keycode))
        return false;
    al_ustr_append_chr(t->input_string, keycode);
    return true;

}


void insert_newline_in_tree(struct TREE *t, const char direction, const unsigned int steps)
{

    /* add newline to active block in tree */
    struct BLOCK *b = active_block(t);
    make_newline_in_block(b,  direction, steps);

}



bool remove_char_from_tree(struct TREE *t, const char direction, const unsigned int steps)
{

    /* remove character from all cursor positions in active block */
    struct BLOCK *b = active_block(t);
    return remove_char_from_block(b,  direction, steps);

}



struct TREE *make_tree(const char *path)
{

    /* create new tree structure
     * called at program load 
     * if buffer is not null, load it into block
     * otherwise add a default block and a default
     * first line, with insertion active
     */

    struct TREE *t = calloc(1, sizeof(struct TREE));
    struct BLOCK *b = NULL;
    if (path)
        b = make_block_from_file(path);
    else
        b = make_empty_block();
    add_cursor_point(&b->data[0]->cursor, 0);
    t->filename = NULL;
    t->block = NULL;
    t->command_string = al_ustr_new("");
    t->input_string = al_ustr_new("");
    t->active_block = 0;
    t->count = 0;
    append_block_to_tree(t, b);
    return t;

}


void kill_tree(struct TREE *t)
{

    /* remove tree from memory
     * called at program exit
     */
    /* 
    unsigned int i;
    for (i = 0; i < t->count; ++t) {
        //kill_block(t->block[i]);
        if (t->command_string)
            al_ustr_free(t->command_string);
        if (t->input_string)
            al_ustr_free(t->input_string);
    }
    */
    free(t);

}



