#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <cjson/cJSON.h>
#include <cjson/cJSON_Utils.h>
#include <stdio.h>
#include <stdlib.h>
#include "Font.h"
#include "Deck.h"
#include "Widgets.h"
#include "utils.h"


// settings
// --------
cJSON *config = NULL;
int debug;
char *appname;
char *winTitle;
int WIDTH; // winSize[0]
int HEIGHT; // winSize[1]
ALLEGRO_COLOR bgColor;
int boardMargin;
int cardsMargin;
Label scoreLabel;
Button resetButton;
Button randomButton;
Button debugButton;
float TIMER_FLIPBACK;
float SELECTED_SCALE;
ConfirmationPrompt confPrompt;

// resources
// ---------
// font is "null-terminated":
// if fonts[i].size == 0 and/or fonts[i].font == NULL, 
// then fonts has length i
Font fonts[MAX_FONTS + 1] = { { 0,NULL } };


// ---
// ---
// original
int SWID;
int SHEI;

// scaled
float scale;
float DWID;
float DHEI;


ALLEGRO_DISPLAY *window;
ALLEGRO_EVENT_QUEUE *eventQueue;
ALLEGRO_TIMER *timer;

ALLEGRO_COLOR alphaTint;
ALLEGRO_COLOR buttonColor;
ALLEGRO_COLOR buttonTextColor;

ALLEGRO_BITMAP *cardsImg;

Deck deck;
int running;
int maxScore, currScore;
int mouseX, mouseY;
int timerActive;
int isDebug = 0;
bool confPromptActive = false;
// ---
// ---



void init();
void resetGame();
void processEvent();
void draw();
void card_value_img_region(int value, float *sx, float *sy);

void deck_draw(Deck *d);


int deck_xpos_f(int i);
int deck_ypos_f(int i);
void load_settings(bool first);

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

        // CHECK correct?
        // TODO: see this https://github.com/liballeg/allegro_wiki/wiki/Allegro-Vivace-%E2%80%93-Basic-game-structure
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
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    window = al_create_display(800, 600);
    eventQueue = al_create_event_queue();

    al_register_event_source(eventQueue, al_get_mouse_event_source());
    al_register_event_source(eventQueue, al_get_display_event_source(window));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());


    //
    alphaTint = al_map_rgba(128, 128, 128, 128);
    buttonColor = al_map_rgb(0, 0, 128);
    buttonTextColor = al_map_rgb(255, 255, 255);

    cardsImg = al_load_bitmap("assets/cards.png");
    SWID = al_get_bitmap_width(cardsImg) / 11;
    SHEI = al_get_bitmap_height(cardsImg) / 3;

    load_settings(true);


    timer = al_create_timer(TIMER_FLIPBACK);
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));

    scoreLabel.y = 2 * boardMargin + 8 * cardsMargin + 4 * DHEI + scoreLabel.marginTop;

    resetButton.y = scoreLabel.y + al_get_font_line_height(scoreLabel.font) + resetButton.marginTop;
    randomButton.y = resetButton.y + resetButton.h + randomButton.marginTop;
    debugButton.y = randomButton.y + randomButton.h + debugButton.marginTop;

    deck_create(&deck);

    running = 1;
    resetGame();
}

void resetGame()
{
    currScore = 20;
    maxScore = 0;
    sprintf(scoreLabel.text, scoreLabel.format, currScore, maxScore);
    deck_shuffle(&deck, &deck_xpos_f, &deck_ypos_f);
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

            if (!confPromptActive) {

                if (rect_contains(WIDTH, 2 * boardMargin + 4 * (2 * cardsMargin + DHEI), 0, 0, mouseX, mouseY) && !timerActive) {
                    bool res = deck_reveal_card(&deck, mouseX, mouseY, DWID, DHEI);
                    if (res) {
                        al_start_timer(timer);
                        timerActive = 1;
                    }
                    continue;
                }

                if (rect_contains(resetButton.w, resetButton.h, resetButton.x, resetButton.y, mouseX, mouseY)) {
                    //resetGame();
                    confPromptActive = true;
                    continue;
                }

                if (rect_contains(randomButton.w, randomButton.h, randomButton.x, randomButton.y, mouseX, mouseY)) {
                    bool res = deck_reveal_random_card(&deck, DWID, DHEI);
                    if (res) {
                        al_start_timer(timer);
                        timerActive = 1;
                    }
                    continue;
                }

                if (rect_contains(debugButton.w, debugButton.h, debugButton.x, debugButton.y, mouseX, mouseY)) {
                    isDebug = !isDebug;
                    debugButton.alt = isDebug;
                    continue;
                }
            }
            else {
                if (rect_contains(confPrompt.yes.w, confPrompt.yes.h, confPrompt.yes.x, confPrompt.yes.y, mouseX, mouseY)) {
                    resetGame();
                    confPromptActive = false;
                }
                else if (rect_contains(confPrompt.no.w, confPrompt.no.h, confPrompt.no.x, confPrompt.no.y, mouseX, mouseY)) {
                    confPromptActive = false;
                }
            }
        }

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                running = 0;
                break;
            case ALLEGRO_KEY_F5:
                load_settings(false);
                break;
            }
            continue;
        }

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (ev.timer.source == timer) {
                int j = -1, k = -1;
                int discarded = 0;
                for (int i = 0; i < 40; i++) {
                    if (deck.cards[i].state == CSTATE_PENDING_DISCARD) {
                        deck.cards[i].state = CSTATE_DISCARDED;
                        discarded = 1;
                        if (j == -1) {
                            j = i;
                        }
                        else {
                            k = i;
                        }
                    }
                    else if (deck.cards[i].state == CSTATE_PENDING_FLIPBACK) {
                        deck.cards[i].state = CSTATE_FACEDOWN;
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
                sprintf(scoreLabel.text, scoreLabel.format, currScore, maxScore);
            }
        }
    }
}

void draw()
{
    deck_draw(&deck);
    label_draw(&scoreLabel);
    button_draw(&resetButton);
    button_draw(&randomButton);
    button_draw(&debugButton);
    if (confPromptActive) {
        int x = confPrompt.x;
        int y = confPrompt.y;
        al_draw_filled_rectangle(x, y, x + confPrompt.w, y + confPrompt.h, confPrompt.bgColor);
        label_draw(&confPrompt.label);
        button_draw(&confPrompt.yes);
        button_draw(&confPrompt.no);
    }
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
        case CSTATE_FACEDOWN:
            if (rect_contains(DWID, DHEI, ci->xpos, ci->ypos, state.x, state.y) && !confPromptActive) {
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

        case CSTATE_PENDING_DISCARD:
        case CSTATE_PENDING_FLIPBACK:
        case CSTATE_FACEUP:
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







int deck_xpos_f(int i)
{
    return boardMargin + cardsMargin + (2 * cardsMargin + DWID) * (i % 10);
}

int deck_ypos_f(int i)
{
    return boardMargin + cardsMargin + (2 * cardsMargin + DHEI) * (i / 10);
}

// TODO: there appears to be a small memory leak here
void load_settings(bool first)
{
    // read entire file into memory
    FILE *f = fopen("settings.json", "r");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *settings = malloc(fsize + 1);
    fread(settings, 1, fsize, f);
    fclose(f);
    settings[fsize] = 0;

    if (config != NULL)
        cJSON_Delete(config);
    config = cJSON_Parse(settings);
    free(settings);

    WIDTH = cJSONUtils_GetPointer(config, "/winSize/0")->valueint;
    HEIGHT = cJSONUtils_GetPointer(config, "/winSize/1")->valueint;
    al_resize_display(window, WIDTH, HEIGHT);

    char *appname = cJSONUtils_GetPointer(config, "/appName")->valuestring;
    char *windowTitle = cJSONUtils_GetPointer(config, "/winTitle")->valuestring;
    al_set_app_name(appname);
    al_set_window_title(window, windowTitle);

    int r, g, b;
    r = cJSONUtils_GetPointer(config, "/bgColor/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/bgColor/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/bgColor/2")->valueint;
    bgColor = al_map_rgb(r, g, b);

    boardMargin = cJSONUtils_GetPointer(config, "/boardMargin")->valueint;
    cardsMargin = cJSONUtils_GetPointer(config, "/cardsMargin")->valueint;

    DWID = (WIDTH - 2 * boardMargin) / (float)10 - 2 * cardsMargin;
    DHEI = SHEI / (float)SWID * DWID;
    for (int i = 0; i < 40; i++) {
        deck.cards[i].xpos = deck_xpos_f(i);
        deck.cards[i].ypos = deck_ypos_f(i);
    }

    // CHECK: how do I know if this algorithm is correct? testing?
    // previous fonts are marked for clearing
    mark_fonts_for_clear(fonts);

    scoreLabel.format = cJSONUtils_GetPointer(config, "/scoreLabel/format")->valuestring;
    int scoreFontSize = cJSONUtils_GetPointer(config, "/scoreLabel/fontSize")->valueint;
    r = cJSONUtils_GetPointer(config, "/scoreLabel/color/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/scoreLabel/color/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/scoreLabel/color/2")->valueint;
    scoreLabel.color = al_map_rgb(r, g, b);

    scoreLabel.x = cJSONUtils_GetPointer(config, "/scoreLabel/posX")->valueint;
    scoreLabel.marginTop = cJSONUtils_GetPointer(config, "/scoreLabel/marginTop")->valueint;
    scoreLabel.y = 2 * boardMargin + 8 * cardsMargin + 4 * DHEI + scoreLabel.marginTop;

    // add new font
    scoreLabel.font = add_font(fonts, scoreFontSize);


    // --

    resetButton.x = cJSONUtils_GetPointer(config, "/btnReset/posX")->valueint;
    resetButton.marginTop = cJSONUtils_GetPointer(config, "/btnReset/marginTop")->valueint;
    resetButton.y = scoreLabel.y + al_get_font_line_height(scoreLabel.font) + resetButton.marginTop;
    resetButton.w = cJSONUtils_GetPointer(config, "/btnReset/size/0")->valueint;
    resetButton.h = cJSONUtils_GetPointer(config, "/btnReset/size/1")->valueint;
    resetButton.fontSize = cJSONUtils_GetPointer(config, "/btnReset/fontSize")->valueint;
    resetButton.font = add_font(fonts, resetButton.fontSize);
    r = cJSONUtils_GetPointer(config, "/btnReset/color/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/btnReset/color/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/btnReset/color/2")->valueint;
    resetButton.color = al_map_rgb(r, g, b);
    r = cJSONUtils_GetPointer(config, "/btnReset/bgColor/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/btnReset/bgColor/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/btnReset/bgColor/2")->valueint;
    resetButton.bgColor = al_map_rgb(r, g, b);
    strcpy(resetButton.text, cJSONUtils_GetPointer(config, "/btnReset/text")->valuestring);

    randomButton.x = cJSONUtils_GetPointer(config, "/btnRandom/posX")->valueint;
    randomButton.marginTop = cJSONUtils_GetPointer(config, "/btnRandom/marginTop")->valueint;
    randomButton.y = resetButton.y + resetButton.h + randomButton.marginTop;
    randomButton.w = cJSONUtils_GetPointer(config, "/btnRandom/size/0")->valueint;
    randomButton.h = cJSONUtils_GetPointer(config, "/btnRandom/size/1")->valueint;
    randomButton.fontSize = cJSONUtils_GetPointer(config, "/btnRandom/fontSize")->valueint;
    randomButton.font = add_font(fonts, randomButton.fontSize);
    r = cJSONUtils_GetPointer(config, "/btnRandom/color/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/btnRandom/color/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/btnRandom/color/2")->valueint;
    randomButton.color = al_map_rgb(r, g, b);
    r = cJSONUtils_GetPointer(config, "/btnRandom/bgColor/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/btnRandom/bgColor/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/btnRandom/bgColor/2")->valueint;
    randomButton.bgColor = al_map_rgb(r, g, b);
    strcpy(randomButton.text, cJSONUtils_GetPointer(config, "/btnRandom/text")->valuestring);


    debugButton.x = cJSONUtils_GetPointer(config, "/btnDebug/posX")->valueint;
    debugButton.marginTop = cJSONUtils_GetPointer(config, "/btnDebug/marginTop")->valueint;
    debugButton.y = randomButton.y + randomButton.h + debugButton.marginTop;
    debugButton.w = cJSONUtils_GetPointer(config, "/btnDebug/size/0")->valueint;
    debugButton.h = cJSONUtils_GetPointer(config, "/btnDebug/size/1")->valueint;
    debugButton.fontSize = cJSONUtils_GetPointer(config, "/btnDebug/fontSize")->valueint;
    debugButton.font = add_font(fonts, debugButton.fontSize);
    r = cJSONUtils_GetPointer(config, "/btnDebug/color/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/btnDebug/color/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/btnDebug/color/2")->valueint;
    debugButton.color = al_map_rgb(r, g, b);
    r = cJSONUtils_GetPointer(config, "/btnDebug/bgColor/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/btnDebug/bgColor/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/btnDebug/bgColor/2")->valueint;
    debugButton.bgColor = al_map_rgb(r, g, b);
    strcpy(debugButton.text, cJSONUtils_GetPointer(config, "/btnDebug/text")->valuestring);
    strcpy(debugButton.textAlt, cJSONUtils_GetPointer(config, "/btnDebug/textAlt")->valuestring);

    //

    TIMER_FLIPBACK = cJSONUtils_GetPointer(config, "/timerFlipback")->valuedouble;
    SELECTED_SCALE = cJSONUtils_GetPointer(config, "/selectedScale")->valuedouble;

    //



    confPrompt.w = cJSONUtils_GetPointer(config, "/confPrompt/size/0")->valueint;
    confPrompt.h = cJSONUtils_GetPointer(config, "/confPrompt/size/1")->valueint;

    // define x and y such that the prompt is centered
    confPrompt.x = (WIDTH - confPrompt.w) / 2;
    confPrompt.y = (HEIGHT - confPrompt.h) / 2;

    int x = confPrompt.x;
    int y = confPrompt.y;

    r = cJSONUtils_GetPointer(config, "/confPrompt/bgColor/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/confPrompt/bgColor/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/confPrompt/bgColor/2")->valueint;
    confPrompt.bgColor = al_map_rgb(r, g, b);
    confPrompt.label.x = cJSONUtils_GetPointer(config, "/confPrompt/label/pos/0")->valueint + x;
    confPrompt.label.y = cJSONUtils_GetPointer(config, "/confPrompt/label/pos/1")->valueint + y;
    confPrompt.label.fontSize = cJSONUtils_GetPointer(config, "/confPrompt/label/fontSize")->valueint;
    confPrompt.label.font = add_font(fonts, confPrompt.label.fontSize);
    r = cJSONUtils_GetPointer(config, "/confPrompt/label/color/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/confPrompt/label/color/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/confPrompt/label/color/2")->valueint;
    confPrompt.label.color = al_map_rgb(r, g, b);
    strcpy(confPrompt.label.text, cJSONUtils_GetPointer(config, "/confPrompt/label/text")->valuestring);

    // Button "Yes"
    confPrompt.yes.x = cJSONUtils_GetPointer(config, "/confPrompt/btnYes/pos/0")->valueint + x;
    confPrompt.yes.y = cJSONUtils_GetPointer(config, "/confPrompt/btnYes/pos/1")->valueint + y;
    confPrompt.yes.fontSize = cJSONUtils_GetPointer(config, "/confPrompt/btnYes/fontSize")->valueint;
    confPrompt.yes.font = add_font(fonts, confPrompt.yes.fontSize);
    r = cJSONUtils_GetPointer(config, "/confPrompt/btnYes/color/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/confPrompt/btnYes/color/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/confPrompt/btnYes/color/2")->valueint;
    confPrompt.yes.color = al_map_rgb(r, g, b);
    r = cJSONUtils_GetPointer(config, "/confPrompt/btnYes/bgColor/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/confPrompt/btnYes/bgColor/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/confPrompt/btnYes/bgColor/2")->valueint;
    confPrompt.yes.bgColor = al_map_rgb(r, g, b);
    confPrompt.yes.w = cJSONUtils_GetPointer(config, "/confPrompt/btnYes/size/0")->valueint;
    confPrompt.yes.h = cJSONUtils_GetPointer(config, "/confPrompt/btnYes/size/1")->valueint;
    strcpy(confPrompt.yes.text, cJSONUtils_GetPointer(config, "/confPrompt/btnYes/text")->valuestring);

    // Button "No"
    confPrompt.no.x = cJSONUtils_GetPointer(config, "/confPrompt/btnNo/pos/0")->valueint + x;
    confPrompt.no.y = cJSONUtils_GetPointer(config, "/confPrompt/btnNo/pos/1")->valueint + y;
    confPrompt.no.fontSize = cJSONUtils_GetPointer(config, "/confPrompt/btnNo/fontSize")->valueint;
    confPrompt.no.font = add_font(fonts, confPrompt.no.fontSize);
    r = cJSONUtils_GetPointer(config, "/confPrompt/btnNo/color/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/confPrompt/btnNo/color/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/confPrompt/btnNo/color/2")->valueint;
    confPrompt.no.color = al_map_rgb(r, g, b);
    r = cJSONUtils_GetPointer(config, "/confPrompt/btnNo/bgColor/0")->valueint;
    g = cJSONUtils_GetPointer(config, "/confPrompt/btnNo/bgColor/1")->valueint;
    b = cJSONUtils_GetPointer(config, "/confPrompt/btnNo/bgColor/2")->valueint;
    confPrompt.no.bgColor = al_map_rgb(r, g, b);
    confPrompt.no.w = cJSONUtils_GetPointer(config, "/confPrompt/btnNo/size/0")->valueint;
    confPrompt.no.h = cJSONUtils_GetPointer(config, "/confPrompt/btnNo/size/1")->valueint;
    strcpy(confPrompt.no.text, cJSONUtils_GetPointer(config, "/confPrompt/btnNo/text")->valuestring);


    // clear marked fonts
    clear_fonts(fonts);
}


