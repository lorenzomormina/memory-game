#include "event.h"
#include "globals.h"

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
                    confPromptActive = true;
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
                load_settings();
                break;
            case ALLEGRO_KEY_TILDE:
                consoleActive = !consoleActive;
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
            else if (ev.timer.source == fpsTimer) {
                al_clear_to_color(bgColor);
                draw();
                al_flip_display();
            }
        }

        if (ev.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            if (consoleActive) {
                if (ev.keyboard.unichar >= 32 && ev.keyboard.unichar <= 126) {
                    if (ev.keyboard.unichar == 96) {
                        continue;
                    }
                    console.cmd[console.cmdSize++] = ev.keyboard.unichar;
                    console.cmd[console.cmdSize] = 0;
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                    if (console.cmdSize > 0) {
                        console.cmd[--console.cmdSize] = 0;
                    }
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    //console_history_add(&console, console.text);
                    if (luaL_dostring(L, console.cmd) != LUA_OK) {
                        sprintf(console.text, "Error: %s\n", lua_tostring(L, -1));
                    }
                    console.cmd[0] = 0;
                    console.cmdSize = 0;
                }
            }
        }
    }
}



int quit(lua_State *L)
{
    running = 0;
    return 0;
}

int peek_cards(lua_State *L)
{
    // retrieve first parameter, which is a boolean
    int n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "peek_cards: wrong number of arguments");
    }
    if (!lua_isboolean(L, 1)) {
        return luaL_error(L, "peek_cards: wrong argument type");
    }
    isDebug = lua_toboolean(L, 1);

    return 0;
}

int random_card(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 0) {
        return luaL_error(L, "random_card: wrong number of arguments");
    }
    bool res = deck_reveal_random_card(&deck, DWID, DHEI);
    if (res) {
        al_start_timer(timer);
        timerActive = 1;
    }
    return 0;
}


int refresh_ui(lua_State *L)
{
    int n = lua_gettop(L);
    if (n != 0) {
        return luaL_error(L, "refresh_ui: wrong number of arguments");
    }
    load_settings();
    return 0;
}