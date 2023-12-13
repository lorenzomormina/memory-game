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

typedef struct Console {
    int x, y, w, h;
    ALLEGRO_COLOR color;
    ALLEGRO_FONT *font;
    int fontSize;
    ALLEGRO_COLOR fontColor;
    char prompt[10]; // eg: "> "
    char text[64]; // "this is the output of the function/command hello()"
    int cmdSize;
    char cmd[64]; // eg: hello()
    char history[64][10];
    int historyIndex;
    int historySize;
} Console;


void button_draw(Button *b);
void label_draw(Label *label);
void console_draw(Console *c);