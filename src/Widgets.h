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


void button_draw(Button *b)
{
    al_draw_filled_rectangle(b->x, b->y, b->x + b->w, b->y + b->h, b->bgColor);
    if (b->alt) {
        int h = al_get_font_line_height(b->font);
        int w = al_get_text_width(b->font, b->textAlt);
        al_draw_text(b->font, b->color, b->x + (b->w - w) / 2, b->y + (b->h - h) / 2, 0, b->textAlt);
    }
    else {
        int h = al_get_font_line_height(b->font);
        int w = al_get_text_width(b->font, b->text);
        al_draw_text(b->font, b->color, b->x + (b->w - w) / 2, b->y + (b->h - h) / 2, 0, b->text);
    }
}

void label_draw(Label *label)
{
    al_draw_text(label->font, label->color, label->x, label->y, 0, label->text);
}