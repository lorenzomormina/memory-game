#include "Widgets.h"

#include <allegro5/allegro_primitives.h>
#include <stdio.h>

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

void console_draw(Console *c)
{
    al_draw_filled_rectangle(c->x, c->y, c->x + c->w, c->y + c->h, c->color);
    char cmd[80];
    sprintf(cmd, "%s%s", c->prompt, c->cmd);
    al_draw_text(c->font, c->fontColor, c->x + 5, c->y + c->h - 5 - al_get_font_line_height(c->font), 0, cmd);
    al_draw_text(c->font, c->fontColor, c->x + 5, c->y + 5, 0, c->text);
}