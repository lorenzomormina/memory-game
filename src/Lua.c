#include "Lua.h"
#include "globals.h"

void lua_getx_nested(const char *name)
{
    char buffer[256];
    strncpy(buffer, name, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *token = strtok(buffer, ".");
    lua_getglobal(L, token);

    while ((token = strtok(NULL, ".")) != NULL) {
        if (!lua_istable(L, -1)) {
            printf("Error: '%s' is not a table\n", name);
            lua_pop(L, 1);
            return;
        }
        lua_pushstring(L, token);
        lua_gettable(L, -2);
        lua_remove(L, -2); // Remove the previous table from the stack
    }
}

// Function to get an integer value from a nested Lua table
int lua_getxi(const char *name)
{
    lua_getx_nested(name);

    if (!lua_isnumber(L, -1)) {
        printf("Error: '%s' is not a number\n", name);
        lua_pop(L, 1);
        return 0;
    }

    int value = lua_tointeger(L, -1);
    lua_pop(L, 1); // Remove the number from the stack
    return value;
}

// Function to get a string value from a nested Lua table
const char *lua_getxs(const char *name)
{
    lua_getx_nested(name);

    if (!lua_isstring(L, -1)) {
        printf("Error: '%s' is not a string\n", name);
        lua_pop(L, 1);
        return NULL;
    }

    const char *value = lua_tostring(L, -1);
    lua_pop(L, 1); // Remove the string from the stack
    return value;
}

// Function to get a float value from a nested Lua table
float lua_getxf(const char *name)
{
    lua_getx_nested(name);

    if (!lua_isnumber(L, -1)) {
        printf("Error: '%s' is not a number\n", name);
        lua_pop(L, 1);
        return 0;
    }

    float value = lua_tonumber(L, -1);
    lua_pop(L, 1); // Remove the number from the stack
    return value;
}

// get array of integers from a nested Lua table
int *lua_getxi_array(const char *name, int *size)
{
    lua_getx_nested(name);

    if (!lua_istable(L, -1)) {
        printf("Error: '%s' is not a table\n", name);
        lua_pop(L, 1);
        return NULL;
    }

    int len = lua_rawlen(L, -1);
    int *array = malloc(len * sizeof(int));

    for (int i = 0; i < len; i++) {
        lua_rawgeti(L, -1, i + 1);
        if (!lua_isnumber(L, -1)) {
            printf("Error: '%s'[%d] is not a number\n", name, i);
            lua_pop(L, 2);
            free(array);
            return NULL;
        }
        array[i] = lua_tointeger(L, -1);
        lua_pop(L, 1); // Remove the number from the stack
    }

    lua_pop(L, 1); // Remove the table from the stack
    *size = len;
    return array;
}

// get array of integers of fixed size from a nested Lua table
int *lua_getxi_array_fixed(const char *name, int size)
{
    lua_getx_nested(name);

    if (!lua_istable(L, -1)) {
        printf("Error: '%s' is not a table\n", name);
        lua_pop(L, 1);
        return NULL;
    }

    int *array = malloc(size * sizeof(int));

    for (int i = 0; i < size; i++) {
        lua_rawgeti(L, -1, i + 1);
        if (!lua_isnumber(L, -1)) {
            printf("Error: '%s'[%d] is not a number\n", name, i);
            lua_pop(L, 2);
            free(array);
            return NULL;
        }
        array[i] = lua_tointeger(L, -1);
        lua_pop(L, 1); // Remove the number from the stack
    }

    lua_pop(L, 1); // Remove the table from the stack
    return array;
}

// get ith element of an array of integers from a nested Lua table
int lua_getxi_array_at(const char *name, int i)
{
    lua_getx_nested(name);

    if (!lua_istable(L, -1)) {
        printf("Error: '%s' is not a table\n", name);
        lua_pop(L, 1);
        return 0;
    }

    lua_rawgeti(L, -1, i + 1);
    if (!lua_isnumber(L, -1)) {
        printf("Error: '%s'[%d] is not a number\n", name, i);
        lua_pop(L, 2);
        return 0;
    }

    int value = lua_tointeger(L, -1);
    lua_pop(L, 2); // Remove the number and the table from the stack
    return value;
}
