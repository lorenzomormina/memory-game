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


void button_draw(Button *b);

void label_draw(Label *label);