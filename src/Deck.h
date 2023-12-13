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




void array_card_remove(Card *arr, int size, int pos)
{
    for (int i = pos; i < size - 1; i++) {
        arr[i] = arr[i + 1];
    }
}

void deck_create(Deck *d)
{
    for (int i = 0; i < 40; i++)
    {
        d->cards[i].index = i;
        d->cards[i].value = i % 20;
    }
}

void deck_shuffle(Deck *d, int (*x)(int), int (*y)(int))
{
    // [CHECK] improve?

    Card tmpDeck[40];
    for (int i = 0; i < 40; i++) {
        tmpDeck[i] = d->cards[i];
    }

    for (int i = 0; i < 40; i++) {
        int r = rand() % (40 - i);
        d->cards[i].index = tmpDeck[r].index;
        d->cards[i].value = tmpDeck[r].value;
        d->cards[i].state = CSTATE_FACEDOWN;

        d->cards[i].xpos = x(i);
        d->cards[i].ypos = y(i);

        array_card_remove(tmpDeck, 40 - i, r);
    }

    d->lastDown = 39;
    d->lastFlipped = -1;
}



bool deck_reveal_card(Deck *d, int x, int y, int w, int h)
{
    // [CHECK] is for sure inside region?

    // [TODO] click is valid for selected_scaled region
    //float dw = DWID * SELECTED_SCALE;
    //float dh = DHEI * SELECTED_SCALE;
    //float dx;
    //float dy;
    for (int i = 0; i <= d->lastDown; i++)
    {
        Card *ci = &(d->cards[i]);
        //dx = ci->ypos - (dh - DHEI) / 2;
        //dy = ci->ypos - (dh - DHEI) / 2;
        if (ci->state == CSTATE_FACEDOWN && rect_contains(w, h, ci->xpos, ci->ypos, x, y))
        {
            ci->state = CSTATE_FACEUP;
            if (d->lastFlipped == -1) {
                d->lastFlipped = i;
            }
            else {
                Card *cf = &(d->cards[d->lastFlipped]);
                if (cf->value == ci->value) {
                    ci->state = CSTATE_PENDING_DISCARD;
                    cf->state = CSTATE_PENDING_DISCARD;
                    d->lastFlipped = -1;
                }
                else {
                    d->lastFlipped = -1;
                    ci->state = CSTATE_PENDING_FLIPBACK;
                    cf->state = CSTATE_PENDING_FLIPBACK;
                }
                return true;
            }
            return false;
        }
    }
    return false;
}

bool deck_reveal_random_card(Deck *d, int w, int h)
{
    if (d->lastDown >= 0) {
        int i = rand() % (d->lastDown + 1);
        return deck_reveal_card(d, d->cards[i].xpos + 1, d->cards[i].ypos + 1, w, h);
    }
}

void deck_push_to_bottom(Deck *d, int i)
{
    Card c = d->cards[i];
    array_card_remove(d->cards, 40, i);
    d->cards[39] = c;
    d->lastDown--;
}