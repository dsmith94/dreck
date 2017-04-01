#ifndef GUI_H
#define GUI_H


struct TREE;
void make_window(const unsigned int width, const unsigned int height);
void core_window_loop(struct TREE *t);
void kill_window(void);

#endif
