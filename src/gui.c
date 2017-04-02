/*
 * =====================================================================================
 *
 *       Filename:  gui.c
 *
 *    Description:  Graphical front-end for KED
 *
 *        Version:  1.0
 *        Created:  03/29/2017 01:38:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Donald J Smith (dsmith94@gmail.com), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "line.h"
#include "tree.h"
#include "token.h"
#include "block.h"

/* fonts table */
static struct {

    ALLEGRO_FONT *regular;
    ALLEGRO_FONT *bold;
    ALLEGRO_FONT *bold_italic;
    ALLEGRO_FONT *italic;
    int size;

} large;

static struct {

    ALLEGRO_FONT *regular;
    ALLEGRO_FONT *bold;
    ALLEGRO_FONT *bold_italic;
    ALLEGRO_FONT *italic;
    int size;

} medium;

static struct {

    ALLEGRO_FONT *regular;
    ALLEGRO_FONT *bold;
    ALLEGRO_FONT *bold_italic;
    ALLEGRO_FONT *italic;
    int size;

} small;


static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_EVENT_QUEUE *queue = NULL;
static ALLEGRO_TIMER *timer = NULL;

static float screen_width;
static float screen_height;

static const int rows_max = 24;
static const int cols_max = 80;

const float fps = 60;


#define WHITE               al_map_rgb(255,255,255)
#define GRAY                al_map_rgba(100,100,100,200)
#define BLACK               al_map_rgb(0,0,0)


unsigned int viewport_height()
{
    return rows_max;
}


float em(void)
{
    return screen_height * 0.019;
}


float left_margin(void)
{
    return em() * 6;
}


float top_margin(void)
{
    return em() * 4;
}


float right_margin(void)
{
    return screen_width - (em() * 8);
}


float bottom_margin(void)
{
    return screen_height - (em() * 4);
}



static void kill_fonts(void)
{

    /* remove fonts table, at program exit */
    al_destroy_font(large.regular);
    al_destroy_font(large.italic);
    al_destroy_font(large.bold_italic);
    al_destroy_font(large.bold);
    al_destroy_font(medium.regular);
    al_destroy_font(medium.italic);
    al_destroy_font(medium.bold_italic);
    al_destroy_font(medium.bold);
    al_destroy_font(small.regular);
    al_destroy_font(small.italic);
    al_destroy_font(small.bold_italic);
    al_destroy_font(small.bold);

}



static void init_fonts(void)
{

    /* create fonts table */
    const int font_standard_height = em();
    large.size = -(font_standard_height * 2);
    medium.size = -(font_standard_height);
    small.size = -(font_standard_height * 0.5);
    large.regular = al_load_ttf_font("fonts/regular.ttf", large.size, 0);
    large.italic = al_load_ttf_font("fonts/italic.ttf", large.size, 0);
    large.bold_italic = al_load_ttf_font("fonts/bold_italic.ttf", large.size, 0);
    large.bold = al_load_ttf_font("fonts/bold.ttf", large.size, 0);
    medium.regular = al_load_ttf_font("fonts/regular.ttf", medium.size, 0);
    medium.italic = al_load_ttf_font("fonts/italic.ttf", medium.size, 0);
    medium.bold_italic = al_load_ttf_font("fonts/bold_italic.ttf", medium.size, 0);
    medium.bold = al_load_ttf_font("fonts/bold.ttf", medium.size, 0);
    small.regular = al_load_ttf_font("fonts/regular.ttf", small.size, 0);
    small.italic = al_load_ttf_font("fonts/italic.ttf", small.size, 0);
    small.bold_italic = al_load_ttf_font("fonts/bold_italic.ttf", small.size, 0);
    small.bold = al_load_ttf_font("fonts/bold.ttf", 22, 0);

}


void make_window(void)
{

    /* create gui window for text display 
     * while gui is still running, we still have
     * work to do
     */

    ALLEGRO_DISPLAY_MODE display_mode_data;
    al_init();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_get_display_mode(0, &display_mode_data);
    screen_width = display_mode_data.width;
    screen_height = display_mode_data.height;
    timer = al_create_timer(1.0 / fps);
    /*
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    */
    display = al_create_display(screen_width, screen_height);
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());
    init_fonts();
    al_start_timer(timer);
    al_set_target_backbuffer(display);

}


static void show_command_mode(struct TREE *t)
{

    /* if the command string has a pending command, show it in the
     * lower console */
    if (!al_ustr_equal(t->command_string, al_ustr_empty_string())) {
        al_draw_ustr(medium.bold, BLACK, 10, 10, ALLEGRO_ALIGN_LEFT, t->command_string);
    }

}


static void draw_cursors(const struct BLOCK *b, const float x, const float y)
{

    /* draw cursors on active block */
    unsigned int row;
    for (row = b->length; row--; ) {

        /* active cursors */
        struct LINE *line = b->data[row];
        int i;
        for (i = line->cursor.count; i--; ) {
            ALLEGRO_USTR *width_measurement = al_ustr_dup_substr(line->data, 0, al_ustr_offset(line->data, line->cursor.point[i]));
            const float width = al_get_ustr_width(medium.regular, width_measurement);
            const float x1 = (width) + x;
            const float y1 = (row * em()) + y - (b->camera_y * em());
            const float x2 = (width) + x + 3;
            const float y2 = (row * em()) + y + em() - (b->camera_y * em());
            al_draw_filled_rounded_rectangle(x1 - 0.5, y1 - 0.5, 
                    x2 + 0.9, y2 + 0.9, 2, 2, GRAY);
            al_draw_filled_rounded_rectangle(x1, y1, x2, y2, 3, 3, BLACK);
            al_ustr_free(width_measurement);
        }

    }

}


void draw_block(struct BLOCK *b, const float x, const float y)
{

    /* draw one standard block, at x and y */
    const float font_width = al_get_text_width(medium.regular, "X");
    const float width = (font_width * cols_max) + em();
    const float height = (em() * rows_max) + em();
    int row  = 0;
    int rows = rows_max + b->camera_y;
    float dx = x;
    float dy = y;
    ALLEGRO_USTR *buffer = al_ustr_new("");
    if (b->length < rows)
        rows = b->length;
    al_set_clipping_rectangle(x, y, width, height);
    al_clear_to_color(WHITE);
    for (row = b->camera_y; row < rows; ++row) {
        struct LINE *line = b->data[row];
        al_ustr_assign_substr(buffer, line->data, 0, al_ustr_size(line->data));
        al_draw_ustr(medium.regular, BLACK, dx, dy, ALLEGRO_ALIGN_LEFT, buffer);
        dy += em();
    }
    draw_cursors(b, x, y);
    al_set_clipping_rectangle(0, 0, screen_width, screen_height);
    al_draw_textf(medium.regular, BLACK, 20, 400, ALLEGRO_ALIGN_LEFT, "%d", b->focus_cursor);

}


static void draw_tree(struct TREE *t)
{

    /* draw text data tree, followed by overlay */
    struct BLOCK *active = active_block(t);
    draw_block(active, left_margin(), top_margin());

}



static void refresh_window(struct TREE *t)
{

    /* called each timer refresh */
    manage_tree(t);
    al_clear_to_color(WHITE);
    draw_tree(t);
    show_command_mode(t);
    al_flip_display();


}


void accept_key_input(const ALLEGRO_EVENT e, struct TREE *t)
{

    /* draw input from user
     * how input is interpreted depends on current UI mode
     */
    struct TOKEN token = make_token(CMD_MOVE, 1, NULL);
    switch (e.keyboard.keycode) {
        case ALLEGRO_KEY_HOME:
            token.direction = DIR_LEFT;
            token.steps = 0;
            break;
        case ALLEGRO_KEY_END:
            token.direction = DIR_RIGHT;
            token.steps = 0;
            break;
        case ALLEGRO_KEY_DELETE:
            token.command = CMD_DELETE;
            token.direction = DIR_LEFT;
            break;
        case ALLEGRO_KEY_BACKSPACE:
            token.command = CMD_DELETE;
            token.direction = DIR_RIGHT;
            break;
        case ALLEGRO_KEY_TAB:
            token.command = CMD_UNICODE;
            token.uchar = ' ';
            token.steps = 8;
            break;
        case ALLEGRO_KEY_ENTER:
            token.command = CMD_NEWLINE;
            token.direction = DIR_DOWN;
            break;
        case ALLEGRO_KEY_LEFT:
            token.direction = DIR_LEFT;
            break;
        case ALLEGRO_KEY_RIGHT:
            token.direction = DIR_RIGHT;
            break;
        case ALLEGRO_KEY_UP:
            token.direction = DIR_UP;
            break;
        case ALLEGRO_KEY_DOWN:
            token.direction = DIR_DOWN;
            break;
        default:
            token.command = CMD_UNICODE;
            token.uchar = e.keyboard.unichar;
    }
    interpret_token(t, token);

}


void core_window_loop(struct TREE *t)
{

    /* called after main window init -- this is where most of the
     * program happens 
     */
    while(1) {
        ALLEGRO_EVENT e;
        al_wait_for_event(queue, &e);
        if (e.type == ALLEGRO_EVENT_TIMER)
            refresh_window(t);
        if (e.type == ALLEGRO_EVENT_KEY_CHAR)
            accept_key_input(e, t);
        else if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
    }

}


void kill_window(void)
{

    /* kill window
     * called at program close
     */

    kill_fonts();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);

}


