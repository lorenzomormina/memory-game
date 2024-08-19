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

    ALLEGRO_COLOR col = al_map_rgb(0, 0, 255);
    // get substring of cmd from 0 to cursorIndex
    char sub[80];
    strncpy(sub, cmd, c->cursorIndex);
    sub[c->cursorIndex] = '\0';
    int x = c->x + 5 + al_get_text_width(c->font, sub) + al_get_text_width(c->font, c->prompt);


    int y = c->y + c->h - 5 - al_get_font_line_height(c->font);
    int dx = 2;
    int dy = al_get_font_line_height(c->font);
    al_draw_filled_rectangle(x, y, x + dx, y + dy, col);
}


void console_add_char(Console *c, char ch) {
    if (c->historyIndex == c->historySize) {
        if (strlen(c->cmd) < CMD_SIZE - 1) {
            c->cmd[strlen(c->cmd) + 1] = '\0';
            // shift right by cursorIndex
            for (int i = strlen(c->cmd); i > c->cursorIndex; i--) {
                c->cmd[i] = c->cmd[i - 1];
            }
            c->cmd[c->cursorIndex] = ch;
            c->cursorIndex++;
        }
    }
    else {
        strcpy(c->cmd, c->history[c->historyIndex]);
        if (strlen(c->cmd) < CMD_SIZE - 1) {
            c->cmd[strlen(c->cmd) + 1] = '\0';
            // shift right by cursorIndex
            for (int i = strlen(c->cmd); i > c->cursorIndex; i--) {
                c->cmd[i] = c->cmd[i - 1];
            }
            c->cmd[c->cursorIndex] = ch;
            c->cursorIndex++;
        }
        c->historyIndex = c->historySize;
    }
}

int console_cmd_size(Console *c) {
    return strlen(c->cmd);
}

void console_remove_char(Console *c) {
    if (c->historyIndex == c->historySize) {
        if (strlen(c->cmd) > 0) {
            // from cursorIndex to end, shift left
            for (int i = c->cursorIndex - 1; i <= strlen(c->cmd); i++) {
                c->cmd[i] = c->cmd[i + 1];
            }
            // WARN! null termination?
            //c->cmd[strlen(c->cmd) - 1] = '\0';
            c->cursorIndex--;
        }
    }
    else {
        strcpy(c->cmd, c->history[c->historyIndex]);
        if (strlen(c->cmd) > 0) {
            // from cursorIndex to end, shift left
            for (int i = c->cursorIndex - 1; i <= strlen(c->cmd); i++) {
                c->cmd[i] = c->cmd[i + 1];
            }
            // WARN! null termination?
            //c->cmd[strlen(c->cmd) - 1] = '\0';
            c->cursorIndex--;
        }
        c->historyIndex = c->historySize;
    }
}

// TODO: don't put consecutive equal elements in history
// TODO: make sure any element exists only once in history
void console_history_add(Console *c) {
    if (c->historySize < HISTORY_SIZE) {
        strcpy(c->history[c->historySize], c->cmd);
        c->historySize++;
        c->historyIndex = c->historySize;
    }
    else {
        for (int i = 0; i < HISTORY_SIZE - 1; i++) {
            strcpy(c->history[i], c->history[i + 1]);
        }
        strcpy(c->history[HISTORY_SIZE - 1], c->cmd);
    }
    c->cursorIndex = 0;

    // print history
    //for (int i = 0; i < c->historySize; i++) {
    //    printf("%s\n", c->history[i]);
    //}
    //puts("");
}

char *console_get_cmd(Console *c) {
    return c->cmd;
}


void console_move_index_up(Console *c)
{
    if (c->historyIndex > 0) {
        c->historyIndex--;
        strcpy(c->cmd, c->history[c->historyIndex]);
    }
    c->cursorIndex = strlen(c->cmd);
}

void console_move_index_down(Console *c)
{
    if (c->historyIndex < c->historySize) {
        c->historyIndex++;
        strcpy(c->cmd, c->history[c->historyIndex]);
    }
    else {
        c->cmd[0] = '\0';
    }
    c->cursorIndex = strlen(c->cmd);
}

bool console_is_last_index(Console *c)
{
    return false;
}

void console_move_cursor_left(Console *c)
{
    if (c->cursorIndex > 0) {
        c->cursorIndex--;
    }
}

void console_move_cursor_right(Console *c)
{
    if (c->cursorIndex < console_cmd_size(c)) {
        c->cursorIndex++;
    }
}

void console_remove_char_right(Console *c) {
    if (c->historyIndex == c->historySize) {
        if (strlen(c->cmd) > 0) {
            // from cursorIndex to end, shift left
            for (int i = c->cursorIndex; i < strlen(c->cmd); i++) {
                c->cmd[i] = c->cmd[i + 1];
            }
            // WARN! null termination?
                //c->cmd[strlen(c->cmd) - 1] = '\0';
        }
    }
    else {
        strcpy(c->cmd, c->history[c->historyIndex]);
        if (strlen(c->cmd) > 0) {
            // from cursorIndex to end, shift left
            for (int i = c->cursorIndex; i < strlen(c->cmd); i++) {
                c->cmd[i] = c->cmd[i + 1];
            }
            // WARN! null termination?
            //c->cmd[strlen(c->cmd) - 1] = '\0';
        }
        c->historyIndex = c->historySize;
    }
}

