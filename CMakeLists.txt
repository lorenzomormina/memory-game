cmake_minimum_required(VERSION 3.27)
project(main)

set(CMAKE_C_STANDARD 90)

add_executable(main src/main.c)

  find_package(Allegro CONFIG REQUIRED)
  target_link_libraries(main PRIVATE Allegro::allegro Allegro::allegro_ttf Allegro::allegro_font Allegro::allegro_primitives Allegro::allegro_image Allegro::allegro_main)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})