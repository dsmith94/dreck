
#include <stdio.h>
#include <stdlib.h>

#include "block.h"
#include "line.h"

#include "tree.h"
#include "gui.h"




int main(int argc, char *argv[])
{

    ALLEGRO_USTR *buffer = NULL;
    struct TREE *t = NULL;
    make_window(1000, 1000);
    if (argc > 1) {
        t = make_tree(argv[1]);
    } else {
        t = make_tree(NULL);
    }
    if (buffer)
        al_ustr_free(buffer);
    core_window_loop(t);
    kill_window();
    kill_tree(t);
    return 0;

}



