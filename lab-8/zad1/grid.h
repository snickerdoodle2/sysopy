#pragma once
#include <stdbool.h>

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col);
void update_grid(char *src, char *dst);
void create_threads();
void destroy_threads();
