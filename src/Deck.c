#include "Deck.h"

void array_card_remove(Card *arr, int size, int pos)
{
    for (int i = pos; i < size - 1; i++) {
        arr[i] = arr[i + 1];
    }
}

void deck_init(Deck *d, int DWID, int DHEI, int SWID, int SHEI, float SELECTED_SCALE, ALLEGRO_COLOR alphaTint, ALLEGRO_BITMAP *cardsImg)
{
    d->DWID = DWID;
    d->DHEI = DHEI;
    d->SWID = SWID;
    d->SHEI = SHEI;
    d->SELECTED_SCALE = SELECTED_SCALE;
    d->alphaTint = alphaTint;
    d->cardsImg = cardsImg;
}

void deck_create(Deck *d)
{
    for (int i = 0; i < 40; i++)
    {
        d->cards[i].index = i;
        d->cards[i].value = i % 20;
    }
}

void deck_shuffle(Deck *d, int (*x)(Deck *, int, int, int), int (*y)(Deck *, int, int, int), int boardMargin, int cardMargin)
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

        d->cards[i].xpos = x(d, i, boardMargin, cardMargin);
        d->cards[i].ypos = y(d, i, boardMargin, cardMargin);

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



// [TODO] card remains selected if mouse is over selected_scaled region
void deck_draw(Deck *d, int mx, int my, bool enableScaled, bool isDebug)
{
    for (int i = 0; i < 40; i++)
    {
        Card *ci = &(d->cards[i]);
        float sx, sy;
        float dw, dh;
        float dx, dy;
        card_value_img_region(d, ci->value, &sx, &sy);
        switch (ci->state)
        {
        case CSTATE_FACEDOWN:
            if (rect_contains(d->DWID, d->DHEI, ci->xpos, ci->ypos, mx, my) && !enableScaled) {
                dw = d->DWID * d->SELECTED_SCALE;
                dh = d->DHEI * d->SELECTED_SCALE;
                dx = ci->xpos - (dw - d->DWID) / 2;
                dy = ci->ypos - (dh - d->DHEI) / 2;
            }
            else {
                dw = d->DWID;
                dh = d->DHEI;
                dx = ci->xpos;
                dy = ci->ypos;
            }
            if (isDebug) {
                al_draw_tinted_scaled_bitmap(d->cardsImg, d->alphaTint, sx, sy, d->SWID, d->SHEI, dx, dy, dw, dh, 0);
            }
            else {
                al_draw_scaled_bitmap(d->cardsImg, 0, 0, d->SWID, d->SHEI, dx, dy, dw, dh, 0);
            }

            break;

        case CSTATE_PENDING_DISCARD:
        case CSTATE_PENDING_FLIPBACK:
        case CSTATE_FACEUP:
            dw = d->DWID;
            dh = d->DHEI;
            dx = ci->xpos;
            dy = ci->ypos;
            al_draw_scaled_bitmap(d->cardsImg, sx, sy, d->SWID, d->SHEI, dx, dy, dw, dh, 0);
            break;
        }
    }
}



void card_value_img_region(Deck *d, int value, float *sx, float *sy)
{
    if (value < 10) {
        *sx = value * d->SWID;
        *sy = d->SHEI;
    }
    else if (value < 20) {
        *sx = (value - 10) * d->SWID;
        *sy = 2 * d->SHEI;
    }
    else {
        *sx = 0;
        *sy = 0;
    }
}







int deck_xpos_f(Deck *d, int i, int boardMargin, int cardsMargin)
{
    return boardMargin + cardsMargin + (2 * cardsMargin + d->DWID) * (i % 10);
}

int deck_ypos_f(Deck *d, int i, int boardMargin, int cardsMargin)
{
    return boardMargin + cardsMargin + (2 * cardsMargin + d->DHEI) * (i / 10);
}