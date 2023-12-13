#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <stdlib.h>


const int WIDTH = 800;
const int HEIGHT = 600;
const int MARGIN = 10;
const float TIMER_FLIPBACK = 0.5;
const int MARGIN_X = 10;
const int MARGIN_Y = 10;
const int BTN_MARGIN = 10;
const int BTN_WID = 100;
const int BTN_HEI = 25;
const float SELECTED_SCALE = 1.2;
const char *appname = "memory";
const char *scoreFormat = "Pairs left: %d. Attempts: %d.";
const char *debugButtonFormat = "Debug [%s]";
const char *debugBtnOn = "ON";
const char *debugBtnOff = "OFF";


typedef enum {
    STATE_FACEDOWN,
    STATE_FACEUP,
    STATE_DISCARDED,
    STATE_PENDING_DISCARD,
    STATE_PENDING_FLIPBACK
} State;

typedef struct
{
    int index;
    int value;
    State state;
    float xpos;
    float ypos;
} Card;

typedef struct {
    Card cards[40];
    int lastDown;
    int lastFlipped;
} Deck;

typedef struct {
    char text[64];
    int x;
    int y;
    int w;
    int h;
} Button;

typedef struct {
    char text[64];
    int x;
    int y;
} Label;


// original
int SWID;
int SHEI;

// scaled
float scale;
int DWID;
int DHEI;


ALLEGRO_DISPLAY *window;
ALLEGRO_EVENT_QUEUE *eventQueue;
ALLEGRO_TIMER *timer;

ALLEGRO_COLOR bgColor;
ALLEGRO_COLOR labelColor;
ALLEGRO_COLOR alphaTint;
ALLEGRO_COLOR buttonColor;
ALLEGRO_COLOR buttonTextColor;

ALLEGRO_FONT *defaultFont;
ALLEGRO_BITMAP *cardsImg;

Deck deck;
Label scoreLabel;
Button resetButton;
Button randomButton;
Button debugButton;
int running;
int maxScore, currScore;
int mouseX, mouseY;
int timerActive;
int isDebug = 0;


void init();
void resetGame();
void processEvent();
void draw();
int rect_contains(int w, int h, int x, int y, int px, int py);
void array_card_remove(Card *arr, int size, int pos);
void card_value_img_region(int value, float *sx, float *sy);

void deck_create(Deck *d);
void deck_shuffle(Deck *d);
void deck_draw(Deck *d);
void deck_reveal_card(Deck *d, int mx, int my);
void deck_reveal_random_card(Deck *d);
void deck_push_to_bottom(Deck *d, int i);
void button_draw(Button *b, ALLEGRO_FONT *font, ALLEGRO_COLOR backgroundColor, ALLEGRO_COLOR textColor);
void label_draw(Label *l, ALLEGRO_FONT *font, ALLEGRO_COLOR color);

int main()
{
    init();

    double lastTime = 0;
    while (running)
    {
        processEvent();

        al_clear_to_color(bgColor);
        draw();
        al_flip_display();

        // [CHECK] correct?
        double currTime = al_get_time();
        if (currTime - lastTime < 1.0 / 60.0) {
            al_rest(1.0 / 60.0 - (currTime - lastTime));
        }
        lastTime = currTime;
    }

    return 0;
}


void init()
{
    //

    srand(time(0));

    al_init();
    al_init_primitives_addon();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    window = al_create_display(WIDTH, HEIGHT);
    eventQueue = al_create_event_queue();
    timer = al_create_timer(TIMER_FLIPBACK);

    al_set_app_name(appname);
    al_set_window_title(window, appname);

    al_register_event_source(eventQueue, al_get_mouse_event_source());
    al_register_event_source(eventQueue, al_get_display_event_source(window));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));

    //

    bgColor = al_map_rgb(150, 150, 150);
    labelColor = al_map_rgb(255, 255, 255);
    alphaTint = al_map_rgba(128, 128, 128, 128);
    buttonColor = al_map_rgb(0, 0, 128);
    buttonTextColor = al_map_rgb(255, 255, 255);

    defaultFont = al_load_ttf_font("assets/RobotoMono-Regular.ttf", 14, 0);
    cardsImg = al_load_bitmap("assets/cards.png");
    SWID = al_get_bitmap_width(cardsImg) / 11;
    SHEI = al_get_bitmap_height(cardsImg) / 3;

    DWID = (WIDTH - 11 * MARGIN) / 10;
    DHEI = SHEI / (float)SWID * DWID;

    deck_create(&deck);

    sprintf(debugButton.text, debugButtonFormat, debugBtnOff);
    debugButton.x = MARGIN_X;
    debugButton.y = HEIGHT - MARGIN_Y - BTN_HEI;
    debugButton.w = BTN_WID;
    debugButton.h = BTN_HEI;

    strcpy(resetButton.text, "Reset");
    resetButton.x = MARGIN_X;
    resetButton.y = debugButton.y - MARGIN_Y - BTN_HEI;
    resetButton.w = BTN_WID;
    resetButton.h = BTN_HEI;

    strcpy(randomButton.text, "Random");
    randomButton.x = MARGIN_X;
    randomButton.y = resetButton.y - MARGIN_Y - BTN_HEI;
    randomButton.w = BTN_WID;
    randomButton.h = BTN_HEI;

    int h = al_get_font_line_height(defaultFont);
    sprintf(scoreLabel.text, scoreFormat, currScore, maxScore);
    scoreLabel.x = MARGIN_X;
    scoreLabel.y = randomButton.y - MARGIN_Y - h;

    running = 1;
    resetGame();
}

void resetGame()
{
    currScore = 20;
    maxScore = 0;
    sprintf(scoreLabel.text, scoreFormat, currScore, maxScore);
    deck_shuffle(&deck);
}

void processEvent()
{
    ALLEGRO_EVENT ev;
    while (al_get_next_event(eventQueue, &ev))
    {
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            running = 0;
            continue;
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            mouseX = ev.mouse.x;
            mouseY = ev.mouse.y;

            if (rect_contains(WIDTH, (DHEI+MARGIN) * 4, 0, 0, mouseX, mouseY) && !timerActive) {
                deck_reveal_card(&deck, mouseX, mouseY);
                continue;
            }

            if (rect_contains(resetButton.w, resetButton.h, resetButton.x, resetButton.y, mouseX, mouseY)) {
                resetGame();
                continue;
            }

            if (rect_contains(randomButton.w, randomButton.h, randomButton.x, randomButton.y, mouseX, mouseY)) {
                deck_reveal_random_card(&deck);
                continue;
            }

            if (rect_contains(debugButton.w, debugButton.h, debugButton.x, debugButton.y, mouseX, mouseY)) {
                isDebug = !isDebug;
                sprintf(debugButton.text, debugButtonFormat, isDebug ? debugBtnOn : debugBtnOff);
                continue;
            }
        }

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                running = 0;
                break;
            }
            continue;
        }

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (ev.timer.source == timer) {
                int j = -1, k = -1;
                int discarded = 0;
                for (int i = 0; i < 40; i++) {
                    if (deck.cards[i].state == STATE_PENDING_DISCARD) {
                        deck.cards[i].state = STATE_DISCARDED;
                        discarded = 1;
                        if (j == -1) {
                            j = i;
                        }
                        else {
                            k = i;
                        }
                    }
                    else if (deck.cards[i].state == STATE_PENDING_FLIPBACK) {
                        deck.cards[i].state = STATE_FACEDOWN;
                    }
                }
                if (j != -1) {
                    deck_push_to_bottom(&deck, j);
                }
                al_stop_timer(timer);
                timerActive = 0;
                if (discarded) {
                    currScore -= 1;
                }
                maxScore += 1;
                sprintf(scoreLabel.text, scoreFormat, currScore, maxScore);
            }
        }
    }
}

void draw()
{
    deck_draw(&deck);
    label_draw(&scoreLabel, defaultFont, labelColor);
    button_draw(&resetButton, defaultFont, buttonColor, buttonTextColor);
    button_draw(&randomButton, defaultFont, buttonColor, buttonTextColor);
    button_draw(&debugButton, defaultFont, buttonColor, buttonTextColor);
}

void deck_create(Deck *d)
{
    for (int i = 0; i < 40; i++)
    {
        d->cards[i].index = i;
        d->cards[i].value = i % 20;
    }
}

void deck_shuffle(Deck *d)
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
        d->cards[i].state = STATE_FACEDOWN;
        d->cards[i].xpos = MARGIN + DWID * (i % 10) + MARGIN * (i % 10);
        d->cards[i].ypos = MARGIN + DHEI * (int)(i / 10) + MARGIN * (i / 10);
        array_card_remove(tmpDeck, 40 - i, r);
    }

    d->lastDown = 39;
    d->lastFlipped = -1;
}

// [TODO] card remains selected if mouse is over selected_scaled region
void deck_draw(Deck *d)
{
    ALLEGRO_MOUSE_STATE state;
    al_get_mouse_state(&state);
    for (int i = 0; i < 40; i++)
    {
        Card *ci = &(d->cards[i]);
        float sx, sy;
        float dw, dh;
        float dx, dy;
        card_value_img_region(ci->value, &sx, &sy);
        switch (ci->state)
        {
        case STATE_FACEDOWN:
            if (rect_contains(DWID, DHEI, ci->xpos, ci->ypos, state.x, state.y)) {
                dw = DWID * SELECTED_SCALE;
                dh = DHEI * SELECTED_SCALE;
                dx = ci->xpos - (dw - DWID) / 2;
                dy = ci->ypos - (dh - DHEI) / 2;
            }
            else {
                dw = DWID;
                dh = DHEI;
                dx = ci->xpos;
                dy = ci->ypos;
            }
            if (isDebug) {
                al_draw_tinted_scaled_bitmap(cardsImg, alphaTint, sx, sy, SWID, SHEI, dx, dy, dw, dh, 0);
            }
            else {
                al_draw_scaled_bitmap(cardsImg, 0, 0, SWID, SHEI, dx, dy, dw, dh, 0);
            }
            
            break;

        case STATE_PENDING_DISCARD:
        case STATE_PENDING_FLIPBACK:
        case STATE_FACEUP:
            //dw = DWID * SELECTED_SCALE;
            //dh = DHEI * SELECTED_SCALE;
            //dx = ci->xpos - (dw - DWID) / 2;
            //dy = ci->ypos - (dh - DHEI) / 2;
            dw = DWID;
            dh = DHEI;
            dx = ci->xpos;
            dy = ci->ypos;
            al_draw_scaled_bitmap(cardsImg, sx, sy, SWID, SHEI, dx, dy, dw, dh, 0);
            break;
        }
    }
}


void deck_reveal_card(Deck *d, int x, int y)
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
        if (ci->state == STATE_FACEDOWN && rect_contains(DWID, DHEI, ci->xpos, ci->ypos, x, y))
        {
            ci->state = STATE_FACEUP;
            if (d->lastFlipped == -1) {
                d->lastFlipped = i;
            }
            else {
                Card *cf = &(d->cards[d->lastFlipped]);
                if (cf->value == ci->value) {
                    ci->state = STATE_PENDING_DISCARD;
                    cf->state = STATE_PENDING_DISCARD;
                    d->lastFlipped = -1;
                }
                else {
                    d->lastFlipped = -1;
                    ci->state = STATE_PENDING_FLIPBACK;
                    cf->state = STATE_PENDING_FLIPBACK;
                }
                al_start_timer(timer);
                timerActive = 1;
            }
            break;
        }
    }

end_for:

    return;
}

void deck_reveal_random_card(Deck *d)
{
    if (d->lastDown >= 0) {
        int i = rand() % (d->lastDown + 1);
        deck_reveal_card(d, d->cards[i].xpos + 1, d->cards[i].ypos + 1);
    }
}

void deck_push_to_bottom(Deck *d, int i)
{
    Card c = d->cards[i];
    array_card_remove(d->cards, 40, i);
    d->cards[39] = c;
    d->lastDown--;
}

int rect_contains(int w, int h, int x, int y, int px, int py)
{
    return ((y < py && py < y + h) && (x < px && px < x + w));

}

void array_card_remove(Card *arr, int size, int pos)
{
    for (int i = pos; i < size - 1; i++) {
        arr[i] = arr[i + 1];
    }
}

void card_value_img_region(int value, float *sx, float *sy)
{
    if (value < 10) {
        *sx = value * SWID;
        *sy = SHEI;
    }
    else if (value < 20) {
        *sx = (value - 10) * SWID;
        *sy = 2 * SHEI;
    }
    else {
        *sx = 0;
        *sy = 0;
    }
}



void button_draw(Button *b, ALLEGRO_FONT *font, ALLEGRO_COLOR backgroundColor, ALLEGRO_COLOR textColor)
{
    al_draw_filled_rectangle(b->x, b->y, b->x + b->w, b->y + b->h, backgroundColor);
    int w = al_get_text_width(font, b->text);
    int h = al_get_font_line_height(font);
    al_draw_text(font, textColor, b->x + (b->w - w) / 2, b->y + (b->h - h) / 2, 0, b->text);
}

void label_draw(Label *l, ALLEGRO_FONT *font, ALLEGRO_COLOR color)
{
    al_draw_text(font, color, l->x, l->y, 0, l->text);
}
