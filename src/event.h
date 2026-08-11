#pragma once

#include "Lua.h"

void processEvent();
void eval_cards();

int quit(lua_State *L);
int peek_cards(lua_State *L);
int random_card(lua_State *L);
int refresh_ui(lua_State *L);
int random_cards(lua_State *L);
int clear_history(lua_State *L);
int win(lua_State *L);