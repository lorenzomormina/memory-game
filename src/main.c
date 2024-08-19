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
#include "globals.h"
#include "event.h"
#include "Lua.h"


void init();
void resetGame();
void processEvent();
void draw();
void load_settings();


int main()
{
    init();
    while (running)
    {
        processEvent();
    }
    return 0;
}


void init()
{
    L = luaL_newstate();
    luaL_openlibs(L);



    lua_register(L, "quit", quit);
    lua_register(L, "peek_cards", peek_cards);
    lua_register(L, "random_card", random_card);
    lua_register(L, "refresh_ui", refresh_ui);
    //

    srand(time(0));

    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    load_settings(true);

    al_set_app_name(appname);
    window = al_create_display(WIDTH, HEIGHT);
    al_set_window_title(window, winTitle);

    eventQueue = al_create_event_queue();
    al_register_event_source(eventQueue, al_get_mouse_event_source());
    al_register_event_source(eventQueue, al_get_display_event_source(window));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());

    alphaTint = al_map_rgba(128, 128, 128, 128);

    cardsImg = al_load_bitmap("assets/cards.png");
    SWID = al_get_bitmap_width(cardsImg) / 11;
    SHEI = al_get_bitmap_height(cardsImg) / 3;


    DWID = (WIDTH - 2 * boardMargin) / (float)10 - 2 * cardsMargin;
    DHEI = SHEI / (float)SWID * DWID;
    for (int i = 0; i < 40; i++) {
        deck.cards[i].xpos = deck_xpos_f(&deck, i, boardMargin, cardsMargin);
        deck.cards[i].ypos = deck_ypos_f(&deck, i, boardMargin, cardsMargin);
    }

    deck_init(&deck, DWID, DHEI, SWID, SHEI, SELECTED_SCALE, alphaTint, cardsImg);
    

    timer = al_create_timer(TIMER_FLIPBACK);
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));

    // WARN: this setting won't refresh on F5
    fpsTimer = al_create_timer(1.0 / FPS);
    al_register_event_source(eventQueue, al_get_timer_event_source(fpsTimer));
    al_start_timer(fpsTimer);

    scoreLabel.y = 2 * boardMargin + 8 * cardsMargin + 4 * DHEI + scoreLabel.marginTop;

    resetButton.y = scoreLabel.y + al_get_font_line_height(scoreLabel.font) + resetButton.marginTop;

    deck_create(&deck);

    running = 1;
    resetGame();
}

void resetGame()
{
    currScore = 20;
    maxScore = 0;
    sprintf(scoreLabel.text, scoreLabel.format, currScore, maxScore);
    deck_shuffle(&deck, &deck_xpos_f, &deck_ypos_f, boardMargin, cardsMargin);
}

void draw()
{
    ALLEGRO_MOUSE_STATE mouseState;
    al_get_mouse_state(&mouseState);
    deck_draw(&deck, mouseState.x, mouseState.y, confPromptActive, isDebug);
    label_draw(&scoreLabel);
    button_draw(&resetButton);
    if (confPromptActive) {
        int x = confPrompt.x;
        int y = confPrompt.y;
        al_draw_filled_rectangle(x, y, x + confPrompt.w, y + confPrompt.h, confPrompt.bgColor);
        label_draw(&confPrompt.label);
        button_draw(&confPrompt.yes);
        button_draw(&confPrompt.no);
    }
    if (consoleActive) {
        console_draw(&console);
    }
}





// CHECK for memory leak
void load_settings()
{
    int result = luaL_dofile(L, "settings.lua");
    if (result != LUA_OK) {
        const char *errorMessage = lua_tostring(L, -1);
        printf("Error: %s\n", errorMessage);
        return;
    }

    appname = lua_getxs("appName");
    winTitle = lua_getxs("winTitle");
    WIDTH = lua_getxi_array_at("winSize", 0);
    HEIGHT = lua_getxi_array_at("winSize", 1);

    int r, g, b;
    r = lua_getxi_array_at("bgColor", 0);
    g = lua_getxi_array_at("bgColor", 1);
    b = lua_getxi_array_at("bgColor", 2);
    bgColor = al_map_rgb(r, g, b);

    FPS = lua_getxi("frameRate");

    boardMargin = lua_getxi("boardMargin");
    cardsMargin = lua_getxi("cardsMargin");
    TIMER_FLIPBACK = lua_getxf("timerFlipback");
    SELECTED_SCALE = lua_getxf("selectedScale");

    // ---


    // CHECK: how do I know if this algorithm is correct? testing?
    // previous fonts are marked for clearing
    mark_fonts_for_clear(fonts);

    scoreLabel.format = (char *)lua_getxs("scoreLabel.format");
    int scoreFontSize = lua_getxi("scoreLabel.fontSize");
    r = lua_getxi_array_at("scoreLabel.color", 0);
    g = lua_getxi_array_at("scoreLabel.color", 1);
    b = lua_getxi_array_at("scoreLabel.color", 2);
    scoreLabel.color = al_map_rgb(r, g, b);

    scoreLabel.x = lua_getxi("scoreLabel.posX");
    scoreLabel.marginTop = lua_getxi("scoreLabel.marginTop");
    scoreLabel.y = 2 * boardMargin + 8 * cardsMargin + 4 * DHEI + scoreLabel.marginTop;

    // add new font
    scoreLabel.font = add_font(fonts, scoreFontSize);


    // --

    resetButton.x = lua_getxi("btnReset.posX");
    resetButton.marginTop = lua_getxi("btnReset.marginTop");
    resetButton.y = scoreLabel.y + al_get_font_line_height(scoreLabel.font) + resetButton.marginTop;
    resetButton.w = lua_getxi_array_at("btnReset.size", 0);
    resetButton.h = lua_getxi_array_at("btnReset.size", 1);
    resetButton.fontSize = lua_getxi("btnReset.fontSize");
    resetButton.font = add_font(fonts, resetButton.fontSize);
    r = lua_getxi_array_at("btnReset.color", 0);
    g = lua_getxi_array_at("btnReset.color", 1);
    b = lua_getxi_array_at("btnReset.color", 2);
    resetButton.color = al_map_rgb(r, g, b);
    r = lua_getxi_array_at("btnReset.bgColor", 0);
    g = lua_getxi_array_at("btnReset.bgColor", 1);
    b = lua_getxi_array_at("btnReset.bgColor", 2);
    resetButton.bgColor = al_map_rgb(r, g, b);
    strcpy(resetButton.text, lua_getxs("btnReset.text"));


    confPrompt.w = lua_getxi_array_at("confPrompt.size", 0);
    confPrompt.h = lua_getxi_array_at("confPrompt.size", 1);

    // define x and y such that the prompt is centered
    confPrompt.x = (WIDTH - confPrompt.w) / 2;
    confPrompt.y = (HEIGHT - confPrompt.h) / 2;

    int x = confPrompt.x;
    int y = confPrompt.y;

    r = lua_getxi_array_at("confPrompt.bgColor", 0);
    g = lua_getxi_array_at("confPrompt.bgColor", 1);
    b = lua_getxi_array_at("confPrompt.bgColor", 2);
    confPrompt.bgColor = al_map_rgb(r, g, b);
    confPrompt.label.x = lua_getxi_array_at("confPrompt.label.pos", 0) + x;
    confPrompt.label.y = lua_getxi_array_at("confPrompt.label.pos", 1) + y;
    confPrompt.label.fontSize = lua_getxi("confPrompt.label.fontSize");
    confPrompt.label.font = add_font(fonts, confPrompt.label.fontSize);
    r = lua_getxi_array_at("confPrompt.label.color", 0);
    g = lua_getxi_array_at("confPrompt.label.color", 1);
    b = lua_getxi_array_at("confPrompt.label.color", 2);
    confPrompt.label.color = al_map_rgb(r, g, b);
    strcpy(confPrompt.label.text, lua_getxs("confPrompt.label.text"));

    // Button "Yes"
    confPrompt.yes.x = lua_getxi_array_at("confPrompt.btnYes.pos", 0) + x;
    confPrompt.yes.y = lua_getxi_array_at("confPrompt.btnYes.pos", 1) + y;
    confPrompt.yes.fontSize = lua_getxi("confPrompt.btnYes.fontSize");
    confPrompt.yes.font = add_font(fonts, confPrompt.yes.fontSize);
    r = lua_getxi_array_at("confPrompt.btnYes.color", 0);
    g = lua_getxi_array_at("confPrompt.btnYes.color", 1);
    b = lua_getxi_array_at("confPrompt.btnYes.color", 2);
    confPrompt.yes.color = al_map_rgb(r, g, b);
    r = lua_getxi_array_at("confPrompt.btnYes.bgColor", 0);
    g = lua_getxi_array_at("confPrompt.btnYes.bgColor", 1);
    b = lua_getxi_array_at("confPrompt.btnYes.bgColor", 2);
    confPrompt.yes.bgColor = al_map_rgb(r, g, b);
    confPrompt.yes.w = lua_getxi_array_at("confPrompt.btnYes.size", 0);
    confPrompt.yes.h = lua_getxi_array_at("confPrompt.btnYes.size", 1);
    strcpy(confPrompt.yes.text, lua_getxs("confPrompt.btnYes.text"));

    // Button "No"
    confPrompt.no.x = lua_getxi_array_at("confPrompt.btnNo.pos", 0) + x;
    confPrompt.no.y = lua_getxi_array_at("confPrompt.btnNo.pos", 1) + y;
    confPrompt.no.fontSize = lua_getxi("confPrompt.btnNo.fontSize");
    confPrompt.no.font = add_font(fonts, confPrompt.no.fontSize);
    r = lua_getxi_array_at("confPrompt.btnNo.color", 0);
    g = lua_getxi_array_at("confPrompt.btnNo.color", 1);
    b = lua_getxi_array_at("confPrompt.btnNo.color", 2);
    confPrompt.no.color = al_map_rgb(r, g, b);
    r = lua_getxi_array_at("confPrompt.btnNo.bgColor", 0);
    g = lua_getxi_array_at("confPrompt.btnNo.bgColor", 1);
    b = lua_getxi_array_at("confPrompt.btnNo.bgColor", 2);
    confPrompt.no.bgColor = al_map_rgb(r, g, b);
    confPrompt.no.w = lua_getxi_array_at("confPrompt.btnNo.size", 0);
    confPrompt.no.h = lua_getxi_array_at("confPrompt.btnNo.size", 1);
    strcpy(confPrompt.no.text, lua_getxs("confPrompt.btnNo.text"));

    console.x = lua_getxi_array_at("console.pos", 0);
    console.y = lua_getxi_array_at("console.pos", 1);
    console.w = lua_getxi_array_at("console.size", 0);
    console.h = lua_getxi_array_at("console.size", 1);
    console.fontSize = lua_getxi("console.fontSize");
    console.font = add_font(fonts, console.fontSize);
    r = lua_getxi_array_at("console.color", 0);
    g = lua_getxi_array_at("console.color", 1);
    b = lua_getxi_array_at("console.color", 2);
    console.color = al_map_rgb(r, g, b);
    r = lua_getxi_array_at("console.fontColor", 0);
    g = lua_getxi_array_at("console.fontColor", 1);
    b = lua_getxi_array_at("console.fontColor", 2);
    console.fontColor = al_map_rgb(r, g, b);
    strcpy(console.prompt, lua_getxs("console.prompt"));
    console.historyIndex = 0;
    console.historySize = 0;
    console.cmdSize = 0;


    // clear marked fonts
    clear_fonts(fonts);
}


