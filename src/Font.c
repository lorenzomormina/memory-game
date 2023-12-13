#include "Font.h"
#include <allegro5/allegro_ttf.h>

void mark_fonts_for_clear(Font *fonts)
{
    for (int i = 0; i < MAX_FONTS; i++) {
        if (fonts[i].size == 0) {
            return;
        }
        fonts[i].clear = true;
    }
}

void clear_fonts(Font *fonts)
{
    for (int i = 0; i < MAX_FONTS; i++) {
        if (fonts[i].size == 0) {
            return;
        }
        if (fonts[i].clear) {
            al_destroy_font(fonts[i].font);
            // shift all elements to the left
            for (int j = i; j < MAX_FONTS - 1; j++) {
                fonts[j] = fonts[j + 1];
                if (fonts[j].size == 0) {
                    break;
                }
            }
        }
    }
}

ALLEGRO_FONT *add_font(Font *fonts, int size)
{
    int i;
    // check if the font is already loaded
    for (i = 0; i < MAX_FONTS; i++) {
        if (fonts[i].size == size) {
            // this font must not be cleared
            fonts[i].clear = false;
            return fonts[i].font;
        }
        else if (fonts[i].size == 0) {
            break;
        }
    }

    ALLEGRO_FONT *font = al_load_ttf_font("assets/RobotoMono-Regular.ttf", size, 0);
    if (font == NULL) {
        return NULL;
    }

    fonts[i].size = size;
    fonts[i].font = font;
    fonts[i].clear = false;

    // null terminating the array
    fonts[i + 1].size = 0;
    fonts[i + 1].font = NULL;

    return font;
}