#pragma once
#include <windows.h>
#include <iostream>
#include "character.h"

const int WIDTH = 40;
const int HEIGHT = 20;
const int MAX_MAPS = 3; // 좌측(-1), 중앙(0), 우측(1)

struct MapData {
    Object tiles[HEIGHT][WIDTH];
    int map_id;
    char theme_char; // 각 맵의 테마를 나타내는 문자
};

extern MapData maps[MAX_MAPS];
extern int current_map_index; // 현재 맵의 인덱스 (0=좌측, 1=중앙, 2=우측)

void init_maps();
void init_single_map(MapData& map_data, int map_id);
void draw_current_map();
int get_map_index_by_id(int map_id);
MapData& get_current_map();
