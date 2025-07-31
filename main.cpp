#include <iostream>
#include <conio.h>
#include <windows.h>
#include <string>
#include "header/header.h"
#include "header/map.h"
#include "header/character.h"
#include "header/item.h"

// 방향, 현재 맵 id, 현재 위치를 받아 다음 맵 id와 위치를 반환
bool try_map_transition(Direction dir, int& map_id, Position& pos) {
    MapData* current_map = maps[get_map_index_by_id(map_id)];
    auto it = current_map->neighbor_map_ids.find(dir);
    if (it == current_map->neighbor_map_ids.end()) return false;

    int next_map_id = it->second;
    switch (dir) {
        case Direction::LEFT:
            pos.x = WIDTH - 2;
            break;
        case Direction::RIGHT:
            pos.x = 1;
            break;
        case Direction::UP:
            pos.y = HEIGHT - 2;
            break;
        case Direction::DOWN:
            pos.y = 1;
            break;
    }
    map_id = next_map_id;
    return true;
}

int main() {
    
    HANDLE hMapFile = OpenFileMappingA(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        "MySharedMemory"
    );

    if (hMapFile == NULL) {
        std::cerr << "Could not open file mapping object. Is mmf_manager running?" << std::endl;
        return 1;
    }

    SharedData* pSharedData = (SharedData*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        sizeof(SharedData)
    );

    if (pSharedData == NULL) {
        std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    // --- Player Slot System ---
    int player_id = -1;
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        if (!pSharedData->players[i].is_active) {
            player_id = i;
            pSharedData->players[i].is_active = true;
            pSharedData->players[i].pos = { WIDTH / 2, HEIGHT / 2 };
            pSharedData->players[i].current_map_id = 0; // 중앙 맵에서 시작
            pSharedData->players[i].symbol = (i==0) ? "^-^" : "o_o";
            break;
        }
    }

    if (player_id == -1) {
        std::cerr << "All player slots are currently taken. Please try again later." << std::endl;
        UnmapViewOfFile(pSharedData);
        CloseHandle(hMapFile);
        return 1;
    }

    init_screen();
    init_maps();
    
    // 플레이어의 현재 맵에 맞춰 current_map_index 설정
    current_map_index = get_map_index_by_id(pSharedData->players[player_id].current_map_id);

    while (true) {
        clear_screen();
        
        // 현재 플레이어의 맵 인덱스 업데이트
        int player_map_index = get_map_index_by_id(pSharedData->players[player_id].current_map_id);
        current_map_index = player_map_index;
        
        // 현재 맵의 광물로 장애물 업데이트
        MapData& current_map = get_current_map();
        for (int i = 0; i < MAX_MINERALS; ++i) {
            if (pSharedData->minerals[player_map_index][i].map_id == pSharedData->players[player_id].current_map_id) {
                current_map.tiles[pSharedData->minerals[player_map_index][i].pos.y][pSharedData->minerals[player_map_index][i].pos.x].is_Obstacle = 
                    !pSharedData->minerals[player_map_index][i].is_collected;
            }
        }

        draw_current_map();

        // 현재 맵의 광물만 그리기
        for (int i = 0; i < MAX_MINERALS; ++i) {
            if (!pSharedData->minerals[player_map_index][i].is_collected && 
                pSharedData->minerals[player_map_index][i].map_id == pSharedData->players[player_id].current_map_id) {
                char buffer[2] = { pSharedData->minerals[player_map_index][i].symbol, '\0' };
                print_char(pSharedData->minerals[player_map_index][i].pos.x, pSharedData->minerals[player_map_index][i].pos.y, buffer);
            }
        }

        // 같은 맵에 있는 활성 플레이어들만 그리기
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (pSharedData->players[i].is_active && 
                pSharedData->players[i].current_map_id == pSharedData->players[player_id].current_map_id) {
                Player player_obj(pSharedData->players[i].symbol, "white", false, pSharedData->players[i].pos, true);
                draw_player(player_obj);
            }
        }

        std::string score = "Minerals: " + std::to_string(pSharedData->minerals_collected);
        std::string map_info = "Map: " + std::to_string(pSharedData->players[player_id].current_map_id);
        print_char(1, HEIGHT + 1, score.c_str());
        print_char(1, HEIGHT + 2, map_info.c_str());

        flip_screen();

        if (_kbhit()) {
            char ch = _getch();
            Position& current_pos = pSharedData->players[player_id].pos;
            Position next_pos = current_pos;
            bool map_transition = false;
            int new_map_id = pSharedData->players[player_id].current_map_id;
            Direction dir;

            switch (ch) {
                case 72: // Up
                    next_pos.y--;
                    if (next_pos.y <= 0)
                        map_transition = try_map_transition(Direction::UP, new_map_id, next_pos);
                    break;
                case 80: // Down
                    next_pos.y++;
                    if (next_pos.y >= HEIGHT - 1)
                        map_transition = try_map_transition(Direction::DOWN, new_map_id, next_pos);
                    break;
                case 75: // Left
                    next_pos.x--;
                    if (next_pos.x <= 0)
                        map_transition = try_map_transition(Direction::LEFT, new_map_id, next_pos);
                    break;
                case 77: // Right
                    next_pos.x++;
                    if (next_pos.x >= WIDTH - 1)
                        map_transition = try_map_transition(Direction::RIGHT, new_map_id, next_pos);
                    break;
                case ' ': // Spacebar - 광물 수집
                    for (int i = 0; i < MAX_MINERALS; ++i) {
                        if (!pSharedData->minerals[player_map_index][i].is_collected &&
                            pSharedData->minerals[player_map_index][i].map_id == pSharedData->players[player_id].current_map_id) {
                            if (isNearMineral(current_pos,pSharedDate->minerals[player_map_index][i]) {//(abs(current_pos.x - pSharedData->minerals[player_map_index][i].pos.x) <= 1 && current_pos.y == pSharedData->minerals[player_map_index][i].pos.y) || (abs(current_pos.y - pSharedData->minerals[player_map_index][i].pos.y) <= 1 && current_pos.x == pSharedData->minerals[player_map_index][i].pos.x)
                                pSharedData->minerals[player_map_index][i].is_collected = true;
                                if (pSharedData->minerals[player_map_index][i].type == COAL) decreaseLife(pSharedData->players[player_id],player_id); //석탄먹으면 피 1 깎임
                                pSharedData->minerals_collected++;
                            }
                        }
                    }
                    break;
                case 27: // ESC
                    pSharedData->players[player_id].is_active = false;
                    UnmapViewOfFile(pSharedData);
                    CloseHandle(hMapFile);
                    release_screen(); 
                    return 0;
            }

            if (map_transition) {
                pSharedData->players[player_id].current_map_id = new_map_id;
                current_pos = next_pos;
            } else {
                // 기존 이동 처리
                MapData& target_map = maps[get_map_index_by_id(pSharedData->players[player_id].current_map_id)];
                
                // 이동하려는 위치에 미수집 광물이 있는지 체크
                bool isMineralThere = false;
                for (int i = 0; i < MAX_MINERALS; ++i) {
                    if (!pSharedData->minerals[player_map_index][i].is_collected &&
                        pSharedData->minerals[player_map_index][i].map_id == pSharedData->players[player_id].current_map_id &&
                        pSharedData->minerals[player_map_index][i].pos.x == next_pos.x &&
                        pSharedData->minerals[player_map_index][i].pos.y == next_pos.y) {
                        isMineralThere = true;
                        break;
                    }
                }

                if (next_pos.x >= 0 && next_pos.x < WIDTH && next_pos.y >= 0 && next_pos.y < HEIGHT &&
                    !target_map.tiles[next_pos.y][next_pos.x].is_Obstacle && !isMineralThere) {
                    current_pos = next_pos;
                }
            }
        }

        Sleep(1);
    }

    // This part is for safety but might not be reached if exiting via ESC
    pSharedData->players[player_id].is_active = false;
    UnmapViewOfFile(pSharedData);
    CloseHandle(hMapFile);
    release_screen();
    return 0;
}
