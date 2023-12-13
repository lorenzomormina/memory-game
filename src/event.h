#pragma once

#include "Lua.h"

void processEvent();

int quit(lua_State *L);
int peek_cards(lua_State *L);
int random_card(lua_State *L);
int refresh_ui(lua_State *L);