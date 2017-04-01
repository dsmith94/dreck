#ifndef TREE_H
#define TREE_H


struct BLOCK;
struct TREE {

    struct BLOCK **block;
    ALLEGRO_USTR **filename;
    ALLEGRO_USTR *command_string;
    ALLEGRO_USTR *input_string;

    unsigned char active_block;

    unsigned char count;

};



struct TREE *make_tree(const char *path);
void manage_tree(struct TREE *t);
struct BLOCK *active_block(struct TREE *t);
void append_block_to_tree(struct TREE *t, struct BLOCK *b);
bool insert_char_to_tree(struct TREE *t, const int32_t keycode);
bool remove_char_from_tree(struct TREE *t, const char direction, const unsigned int steps);
void insert_newline_in_tree(struct TREE *t, const char direction, const unsigned int steps);
void kill_tree(struct TREE *t);

#endif
