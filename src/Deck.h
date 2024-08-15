#pragma once

#include <allegro5/allegro.h>
#include <stdbool.h>
#include "utils.h"

typedef enum CState {
    CSTATE_FACEDOWN,
    CSTATE_FACEUP,
    CSTATE_DISCARDED,
    CSTATE_PENDING_DISCARD,
    CSTATE_PENDING_FLIPBACK
} State;

typedef struct Card {
    int index;
    int value;
    State state;
    float xpos;
    float ypos;
} Card;

typedef struct Deck {
    Card cards[40];
    int lastDown;
    int lastFlipped;

    int DWID, DHEI;
    int SWID, SHEI;
    float SELECTED_SCALE;
    ALLEGRO_COLOR alphaTint;
    ALLEGRO_BITMAP *cardsImg;
} Deck;


void deck_init(Deck *d, int DWID, int DHEI, int SWID, int SHEI, float SELECTED_SCALE, ALLEGRO_COLOR alphaTint, ALLEGRO_BITMAP *cardsImg);

void array_card_remove(Card *arr, int size, int pos);

void deck_create(Deck *d);

void deck_shuffle(Deck *d, int (*x)(Deck *, int, int, int), int (*y)(Deck *, int, int, int), int boardMargin, int cardMargin);



bool deck_reveal_card(Deck *d, int x, int y, int w, int h);

bool deck_reveal_random_card(Deck *d, int w, int h);

void deck_push_to_bottom(Deck *d, int i);

void deck_draw(Deck *d, int mx, int my, bool enableScaled, bool isDebug);


void card_value_img_region(Deck *d, int value, float *sx, float *sy);
int deck_xpos_f(Deck *d, int i, int boardMargin, int cardsMargin);
int deck_ypos_f(Deck *d, int i, int boardMargin, int cardsMargin);