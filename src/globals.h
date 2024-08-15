#pragma once

#include <allegro5/allegro.h>
#include <cjson/cJSON.h>

#include "Deck.h"
#include "Font.h"
#include "Widgets.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>


// settings
// --------
extern char *appname;
extern char *winTitle;
extern int WIDTH; // winSize[0]
extern int HEIGHT; // winSize[1]
extern ALLEGRO_COLOR bgColor;
extern int FPS;

extern int boardMargin;
extern int cardsMargin;
extern float TIMER_FLIPBACK;
extern float SELECTED_SCALE;

// ----

extern Label scoreLabel;
extern Button resetButton;
extern Button randomButton;
extern Button debugButton;
extern ConfirmationPrompt confPrompt;
extern Console console;

// resources
// ---------
// font is "null-terminated":
// if fonts[i].size == 0 and/or fonts[i].font == NULL, 
// then fonts has length i
extern Font fonts[MAX_FONTS + 1];


// ---
// original
extern int SWID;
extern int SHEI;

// scaled
extern float scale;
extern float DWID;
extern float DHEI;

extern ALLEGRO_DISPLAY *window;
extern ALLEGRO_EVENT_QUEUE *eventQueue;
extern ALLEGRO_TIMER *timer;
extern ALLEGRO_TIMER *fpsTimer;

extern ALLEGRO_COLOR alphaTint;
extern ALLEGRO_COLOR buttonColor;
extern ALLEGRO_COLOR buttonTextColor;

extern ALLEGRO_BITMAP *cardsImg;

extern Deck deck;
extern int running;
extern int maxScore, currScore;
extern int mouseX, mouseY;
extern int timerActive;
extern int isDebug;
extern bool confPromptActive;
extern bool consoleActive;
// ---
// ---

extern lua_State *L;
