#include "../header/header.h"
#include <iostream>
#include <windows.h>
#include "../header/map.h"

HANDLE g_hScreen[2];
int g_nScreenIndex;

void init_screen() {
    CONSOLE_CURSOR_INFO cci;
    cci.dwSize = 1;
    cci.bVisible = FALSE;

    g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleCursorInfo(g_hScreen[0], &cci);
    g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleCursorInfo(g_hScreen[1], &cci);

    g_nScreenIndex = 0;
}

void flip_screen() {
    SetConsoleActiveScreenBuffer(g_hScreen[g_nScreenIndex]);
    g_nScreenIndex = !g_nScreenIndex;
}

void clear_screen() {
    COORD coord = { 0, 0 };
    DWORD dw;
    FillConsoleOutputCharacter(g_hScreen[g_nScreenIndex], ' ', WIDTH * HEIGHT, coord, &dw);
}

void release_screen() {
    CloseHandle(g_hScreen[0]);
    CloseHandle(g_hScreen[1]);
}

void print_char(int x, int y, const char* str) {
    COORD coord = { (SHORT)x, (SHORT)y };
    DWORD dw;
    WriteConsoleOutputCharacterA(g_hScreen[g_nScreenIndex], str, strlen(str), coord, &dw);
}

void HideCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void MoveCursorToOrigin() {
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
