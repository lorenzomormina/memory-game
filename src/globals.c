#include "globals.h"

// settings
// --------
cJSON *config;
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
Console console;
int FPS;
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
ALLEGRO_TIMER *fpsTimer;

ALLEGRO_COLOR alphaTint;
ALLEGRO_COLOR buttonColor;
ALLEGRO_COLOR buttonTextColor;

ALLEGRO_BITMAP *cardsImg;

Deck deck;
int running;
int maxScore, currScore;
int mouseX, mouseY;
int timerActive;
int isDebug;
bool confPromptActive; 
bool consoleActive = false;
// ---
// ---

lua_State *L;