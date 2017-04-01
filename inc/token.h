/*
 * =====================================================================================
 *
 *       Filename:  token.h
 *
 *    Description:  Token header
 *
 *        Version:  1.0
 *        Created:  03/31/2017 12:02:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Donald Smith (dsmith94@gmail.com), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>
#include <allegro5/allegro.h>


#define CMD_MOVE                    'm'
#define CMD_DELETE                  'd'
#define CMD_COPY                    'c'
#define CMD_PASTE                   'v'
#define CMD_CUT                     'x'
#define CMD_BUFFER                  'b'
#define CMD_UNICODE                 'e'
#define CMD_NEWLINE                 'n'

#define DIR_UP                      'u'
#define DIR_DOWN                    'd'
#define DIR_LEFT                    'l'
#define DIR_RIGHT                   'r'


struct TREE;
struct TOKEN {

    ALLEGRO_USTR *string;
    char command;
    char direction;
    unsigned int number;
    unsigned int steps;
    int32_t uchar;

};


struct TOKEN_STRING {

    struct TOKEN *token;

    unsigned int count;

};


struct TOKEN make_token(char cmd, unsigned int steps, ALLEGRO_USTR *string);
bool verify_token_direction(const char d);
bool interpret_token(struct TREE *tree, const struct TOKEN t);
bool verify_token(struct TOKEN t);

#endif

