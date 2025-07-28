#pragma once
#include <windows.h>

void init_screen();
void flip_screen();
void clear_screen();
void release_screen();
void print_char(int x, int y, const char* str);

void HideCursor();
void MoveCursorToOrigin();
