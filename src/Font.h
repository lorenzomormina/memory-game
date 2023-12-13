#pragma once

#include <allegro5/allegro_font.h>
#include <stdbool.h>

#define MAX_FONTS 16

typedef struct Font {
    int size;
    ALLEGRO_FONT *font;
    bool clear;
} Font;

// Font *fonts is an "null-terminated" array

void mark_fonts_for_clear(Font *fonts); ;

void clear_fonts(Font *fonts);

ALLEGRO_FONT *add_font(Font *fonts, int size);