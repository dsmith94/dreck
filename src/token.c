/*
 * =====================================================================================
 *
 *       Filename:  token.c
 *
 *    Description:  Token functions
 *
 *        Version:  1.0
 *        Created:  03/31/2017 12:11:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Donald Smith (dsmith94@gmail.com), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <allegro5/allegro.h>

#include "token.h"
#include "block.h"
#include "tree.h"
#include "line.h"

struct TOKEN make_token(char cmd, unsigned int steps, ALLEGRO_USTR *string)
{

    /* create new token for editor manipulation */
    struct TOKEN t;
    t.command = cmd;
    t.steps = steps;
    t.string = string;
    return t;

}


bool verify_token_direction(const char d)
{

    /* check that direction contained in token is valid
     * if not, then return false
     */
    switch (d) {
        case DIR_UP:
        case DIR_DOWN:
        case DIR_LEFT:
        case DIR_RIGHT:
            return true;
        default:
            return false;
    }
    return false;

}



static bool evaluate_token(struct TREE *tree, const struct TOKEN t)
{

    /* interpret individual token for given tree
     * if token cannot be verified as executable,
     * exit now 
     */
    struct BLOCK *b = active_block(tree);
    switch(t.command) {
        case CMD_MOVE:
            return move_cursor_command(b, t);
        case CMD_UNICODE:
            return insert_char_to_tree(tree, t.uchar);
        case CMD_NEWLINE:
            insert_newline_in_tree(tree, t.direction, t.steps);
            return false;
        case CMD_DELETE:
            return remove_char_from_tree(tree, t.direction, t.steps);
        default:
            return false;
    }
    return false;

}


static bool indefinite_token_evaluation(struct TREE *tree, const struct TOKEN token)
{

    /* evaluate token indefinitely -- when token processing fails,
     * end evaluation 
     */
    while (evaluate_token(tree, token)) {
    }
    return false;

}


static bool looped_token_evaluation(struct TREE *tree, const struct TOKEN token)
{

    /* evaluate token by loop -- when token processing fails, or loop is ended,
     * end evaluation 
     */
    unsigned int i;
    for (i = token.steps; i--; )
        if (!evaluate_token(tree, token))
            return false;
    return false;

}


bool interpret_token(struct TREE *tree, const struct TOKEN token)
{

    /* determine how to interpret token
     * if number of steps is higher than zero, 
     * execute for number of steps
     */
    if (!token.steps)
        return indefinite_token_evaluation(tree, token);
    else
        return looped_token_evaluation(tree, token);

}


struct TOKEN_STRING *make_token_string(void)
{

    /* create new token string */
    struct TOKEN_STRING *t = calloc(1, sizeof(struct TOKEN_STRING));
    t->token = NULL;
    t->count = 0;
    return t;

}



