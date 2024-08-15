#pragma once

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

void lua_getx_nested(const char *name);
int lua_getxi(const char *name);
const char *lua_getxs(const char *name);
float lua_getxf(const char *name);
int *lua_getxi_array(const char *name, int *size);
int *lua_getxi_array_fixed(const char *name, int size);
int lua_getxi_array_at(const char *name, int i);
