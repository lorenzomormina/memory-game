#pragma once

#include <allegro5/allegro_font.h>

typedef struct Button {
    char textAlt[32];
    char text[32];
    int fontSize;
    ALLEGRO_COLOR color, bgColor;
    int w; // size[0]
    int h; // size[1]
    int marginTop;
    bool alt;
    int x;

    int y;
    ALLEGRO_FONT *font;
} Button;

typedef struct Label {
    char *format;
    int fontSize;
    ALLEGRO_COLOR color;
    int x;
    int marginTop;

    char text[64];
    int y;
    int w, h;
    ALLEGRO_FONT *font;
} Label;

typedef struct ConfirmationPrompt {
    int x, y, w, h;
    ALLEGRO_COLOR bgColor;
    Label label;
    Button yes;
    Button no;
} ConfirmationPrompt;

#define PROMPT_SIZE 10
#define CMD_SIZE 64
#define TEXT_SIZE 64
#define HISTORY_SIZE 10

typedef struct Console {
    int x, y, w, h;
    ALLEGRO_COLOR color;
    ALLEGRO_FONT *font;
    int fontSize;
    ALLEGRO_COLOR fontColor;
    char prompt[PROMPT_SIZE]; // eg: "> "
    char cmd[CMD_SIZE]; // "hello"
    char text[TEXT_SIZE]; // "this is the output of the function/command hello()"
    char history[HISTORY_SIZE][CMD_SIZE];
    int historyIndex;
    int historySize;
    int cursorIndex;
} Console;


void button_draw(Button *b);
void label_draw(Label *label);
void console_draw(Console *c);
void console_add_char(Console *c, char ch);
int console_cmd_size(Console *c);
void console_remove_char(Console *c);
void console_history_add(Console *c);
char *console_get_cmd(Console *c);
void console_move_index_up(Console *c);
void console_move_index_down(Console *c);
bool console_is_last_index(Console *c);
void console_move_cursor_left(Console *c);
void console_move_cursor_right(Console *c);
void console_remove_char_right(Console *c);