#pragma once
#include <windows.h>
#include <iostream>
#include "character.h"

const int WIDTH = 50;
const int HEIGHT = 30;
const int MAX_MAPS = 10; // 좌측(-1), 중앙(0), 우측(1)

enum class Direction { LEFT, RIGHT, UP, DOWN };

class MapData {
public:
    int map_id;
    char theme_char;
    Object tiles[HEIGHT][WIDTH];
    std::map<Direction, int> neighbor_map_ids; // 각 방향별 연결된 맵 id

    virtual void init() = 0;
    virtual ~MapData() {}
};

class ForestMap : public MapData {
public:
    ForestMap() { map_id = -1; theme_char = 'T'; }
    void init() override;
};

class MountainMap : public MapData {
public:
    MountainMap() { map_id = 1; theme_char = '^'; }
    void init() override;
};

class LavaMap : public MapData {
public:
    LavaMap() { map_id = 1; theme_char = '0'; }
    void init() override;
};

// ... 필요에 따라 추가

extern MapData maps[MAX_MAPS];
extern int current_map_index; // 현재 맵의 인덱스 (0=좌측, 1=중앙, 2=우측)

void init_maps();
void init_single_map(MapData& map_data, int map_id);
void draw_current_map();
int get_map_index_by_id(int map_id);
MapData& get_current_map();
