#pragma once
#include "character.h"
#include "map.h"

const int MAX_MINERALS = 10;
const int MAX_PLAYERS = 2;

enum MineralType {
    COAL,
    IRON,
    GOLD
};

struct Mineral {
    Position pos;
    MineralType type;
    char symbol;
    bool is_collected;
    int map_id; // 어느 맵에 속하는지
};

struct PlayerData {
    Position pos;
    const char* symbol;
    bool is_active;
    int current_map_id; // 플레이어가 현재 있는 맵
};

struct SharedData {
    Mineral minerals[MAX_MAPS][MAX_MINERALS]; // 맵별 광물
    int minerals_collected;
    PlayerData players[MAX_PLAYERS];
};
