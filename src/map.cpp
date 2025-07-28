#include "../header/map.h"
#include "../header/header.h"

MapData maps[MAX_MAPS];
int current_map_index = 1; // 시작은 중앙 맵 (인덱스 1)

void init_maps() {
    // 좌측 맵 (map_id = -1, index = 0)
    init_single_map(maps[0], -1);
    
    // 중앙 맵 (map_id = 0, index = 1)
    init_single_map(maps[1], 0);
    
    // 우측 맵 (map_id = 1, index = 2)
    init_single_map(maps[2], 1);
}

void init_single_map(MapData& map_data, int map_id) {
    map_data.map_id = map_id;
    
    // 맵별 테마 설정
    switch (map_id) {
        case -1: // 좌측 맵 - 숲 테마
            map_data.theme_char = 'T';
            break;
        case 0:  // 중앙 맵 - 기본 테마
            map_data.theme_char = '#';
            break;
        case 1:  // 우측 맵 - 산 테마
            map_data.theme_char = '^';
            break;
    }
    
    // 기본 맵 구조 생성
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                map_data.tiles[i][j] = { map_data.theme_char, "white", true };
            }
            else {
                map_data.tiles[i][j] = { ' ', "white", false };
            }
        }
    }
    
    // 맵별 고유한 장애물 추가
    if (map_id == -1) { // 좌측 맵 - 나무들 추가
        for (int i = 5; i < 15; i += 3) {
            for (int j = 10; j < 30; j += 5) {
                if (i < HEIGHT - 1 && j < WIDTH - 1) {
                    map_data.tiles[i][j] = { 'T', "white", true };
                }
            }
        }
    }
    else if (map_id == 1) { // 우측 맵 - 바위들 추가
        for (int i = 3; i < 17; i += 4) {
            for (int j = 8; j < 32; j += 6) {
                if (i < HEIGHT - 1 && j < WIDTH - 1) {
                    map_data.tiles[i][j] = { '^', "white", true };
                }
            }
        }
    }
}

void draw_current_map() {
    MapData& current_map = maps[current_map_index];
    
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            Position pos = { j, i };
            draw_object(pos, current_map.tiles[i][j]);
        }
    }
}

int get_map_index_by_id(int map_id) {
    switch (map_id) {
        case -1: return 0; // 좌측 맵
        case 0:  return 1; // 중앙 맵
        case 1:  return 2; // 우측 맵
        default: return 1; // 기본값은 중앙 맵
    }
}

MapData& get_current_map() {
    return maps[current_map_index];
}
