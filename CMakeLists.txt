cmake_minimum_required(VERSION 3.27)
project(main)

set(CMAKE_C_STANDARD 90)

add_executable(main src/main.c "src/Font.h" "src/Deck.h" "src/utils.h" "src/Widgets.h" "src/globals.h" "src/globals.c" "src/utils.c" "src/Widgets.c" "src/Font.c" "src/Deck.c" "src/event.h" "src/event.c" "src/Lua.h" "src/Lua.c")

find_package(Allegro CONFIG REQUIRED)
target_link_libraries(main PRIVATE Allegro::allegro Allegro::allegro_ttf Allegro::allegro_font Allegro::allegro_primitives Allegro::allegro_image Allegro::allegro_main)
find_package(Lua REQUIRED)
target_include_directories(main PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(main PRIVATE ${LUA_LIBRARIES})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/settings.lua DESTINATION ${CMAKE_CURRENT_BINARY_DIR})