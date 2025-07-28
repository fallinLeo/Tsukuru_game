#include <iostream>
#include <conio.h>
#include <windows.h>
#include <string>
#include "header/header.h"
#include "header/map.h"
#include "header/character.h"
#include "header/item.h"

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

            switch (ch) {
                case 72: next_pos.y--; break; // Up
                case 80: next_pos.y++; break; // Down
                case 75: // Left
                    next_pos.x--;
                    // 좌측 경계를 넘어가면 맵 전환
                    if (next_pos.x <= 1) {
                        if (pSharedData->players[player_id].current_map_id > -1) {
                            new_map_id = pSharedData->players[player_id].current_map_id - 1;
                            next_pos.x = WIDTH - 2; // 새 맵의 우측 끝으로
                            map_transition = true;
                        } else {
                            next_pos.x = 0; // 더 이상 갈 수 없음
                        }
                    }
                    break;
                case 77: // Right
                    next_pos.x++;
                    // 우측 경계를 넘어가면 맵 전환
                    if (next_pos.x >= WIDTH-1) {
                        if (pSharedData->players[player_id].current_map_id < 1) {
                            new_map_id = pSharedData->players[player_id].current_map_id + 1;
                            next_pos.x = 1; // 새 맵의 좌측 끝으로
                            map_transition = true;
                        } else {
                            next_pos.x = WIDTH - 1; // 더 이상 갈 수 없음
                        }
                    }
                    break;
                case ' ': // Spacebar - 광물 수집
                    for (int i = 0; i < MAX_MINERALS; ++i) {
                        if (!pSharedData->minerals[player_map_index][i].is_collected &&
                            pSharedData->minerals[player_map_index][i].map_id == pSharedData->players[player_id].current_map_id) {
                            if ((abs(current_pos.x - pSharedData->minerals[player_map_index][i].pos.x) <= 1 && current_pos.y == pSharedData->minerals[player_map_index][i].pos.y) ||
                                (abs(current_pos.y - pSharedData->minerals[player_map_index][i].pos.y) <= 1 && current_pos.x == pSharedData->minerals[player_map_index][i].pos.x)) {
                                pSharedData->minerals[player_map_index][i].is_collected = true;
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
                // 맵 전환 수행
                pSharedData->players[player_id].current_map_id = new_map_id;
                current_pos = next_pos;
            } else {
                // 일반적인 이동 처리
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
