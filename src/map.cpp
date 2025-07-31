#include "../header/map.h"
#include "../header/header.h"

std::vector<MapData*> maps;

void init_maps() {
    maps.push_back(new ForestMap());
    maps.push_back(new CenterMap());
    maps.push_back(new MountainMap());
    // ... 추가
    for (auto* map : maps) map->init();
}

void ForestMap::init() {
    // 기본 구조 및 숲 장애물 배치
    // 나무 장애물 추가
    for (int i = 5; i < 15; i += 3) {
        for (int j = 10; j < 30; j += 5) {
            if (i < HEIGHT - 1 && j < WIDTH - 1) {
                tiles[i][j] = { theme_char, "white", true };
            }
        }
    }
    neighbor_map_ids[Direction::RIGHT] = 0; // 중앙 맵
}

void CenterMap::init() {
    neighbor_map_ids[Direction::LEFT] = -1; // 좌측 맵
    neighbor_map_ids[Direction::RIGHT] = 1; // 우측 맵
    neighbor_map_ids[Direction::DOWN] = 2; // 예시: 아래로 우우측 맵
}

void MountainMap::init() {
    // 기본 구조 및 산 장애물 배치
    // 바위 장애물 추가
    for (int i = 3; i < 17; i += 4) {
        for (int j = 8; j < 32; j += 6) {
            if (i < HEIGHT - 1 && j < WIDTH - 1) {
                tiles[i][j] = { theme_char, "white", true };
            }
        }
    }
    neighbor_map_ids[Direction::LEFT] = 0; // 중앙 맵
}

void LavaMap::init() {
    //용암 맵 --추가
    for (int i = 3; i < HEIGHT - 3; i++) {
        for (int j = 3; j < WIDTH - 3; j++) {
            if (i % 3 == 0 && j % 3 == 0) {
                map_data.tiles[i][j] = { theme_char, "white", true };
            }
        }
    }
    neighbor_map_ids[Direction::UP] = 0; // 중앙 맵
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
        case 2:  return 3; // 우우측 맵
        default: return 1; // 기본값은 중앙 맵
    }
}

MapData& get_current_map() {
    return maps[current_map_index];
}

/*
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
    case 2:
        map_data.theme_char = 'O';
        break;
    }

    // 기본 맵 구조 생성
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                map_data.tiles[i][j] = { map_data.theme_char, "white", true };
                if ((i == HEIGHT / 2) && (i == 0 || i == WIDTH)) map_data.tiles[i][j] = { ' ', "white", false }; //좌 우 뚫어놈.
            }
            else {
                map_data.tiles[i][j] = { ' ', "white", false };
            }
        }
    }

    // 각 맵 타입에 맞는 초기화 호출
    switch (map_id) {
    case -1:
        static_cast<ForestMap&>(map_data).init();
        break;
    case 0:
        // 중앙 맵 초기화 (필요시 추가적인 초기화 코드 삽입)
        break;
    case 1:
        static_cast<MountainMap&>(map_data).init();
        break;
    case 2:
        // 우우측 맵 - 라바 등 추가 (기존 코드 유지)
        for (int i = 3; i < HEIGHT - 3; i++) {
            for (int j = 3; j < WIDTH - 3; j++) {
                if (i % 3 == 0 && j % 3 == 0) {
                    map_data.tiles[i][j] = { map_data.theme_char, "white", true };
                }
            }
        }
        break;
    }
}


*/