#pragma once

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
} Deck;




void array_card_remove(Card *arr, int size, int pos);

void deck_create(Deck *d);

void deck_shuffle(Deck *d, int (*x)(int), int (*y)(int));



bool deck_reveal_card(Deck *d, int x, int y, int w, int h);

bool deck_reveal_random_card(Deck *d, int w, int h);

void deck_push_to_bottom(Deck *d, int i);