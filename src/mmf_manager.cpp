#include <iostream>
#include <windows.h>
#include <time.h>
#include "../header/item.h"
#include "../header/map.h"

int main() {
    srand((unsigned int)time(NULL));

    HANDLE hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(SharedData),
        "MySharedMemory"
    );

    if (hMapFile == NULL) {
        std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
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

    bool isPositionUsed[MAX_MINERALS] = {false};
    int usedPositions[MAX_MINERALS][2] = {0};

    auto isDuplicate = [&](int x,int y,int count){
        for (int i = 0; i < count; ++i) {
            if (isPositionUsed[i] && usedPositions[i][0] == x && usedPositions[i][1] == y) {
                return true;
            }
        }
        return false;
    };

    pSharedData->minerals_collected = 0;

    // Initialize minerals for each map
    for (int map_idx = 0; map_idx < MAX_MAPS; ++map_idx) {
        int map_id = (map_idx == 0) ? -1 : (map_idx == 1) ? 0 : 1; // -1, 0, 1
        
        bool isPositionUsedForMap[MAX_MINERALS] = {false};
        int usedPositionsForMap[MAX_MINERALS][2] = {0};
        
        auto isDuplicateForMap = [&](int x, int y, int count) {
            for (int i = 0; i < count; ++i) {
                if (isPositionUsedForMap[i] && usedPositionsForMap[i][0] == x && usedPositionsForMap[i][1] == y) {
                    return true;
                }
            }
            return false;
        };
        
        for (int i = 0; i < MAX_MINERALS; ++i) {
            int x, y;
            do {
                x = rand() % (WIDTH - 2) + 1;
                y = rand() % (HEIGHT - 2) + 1;
            } while (isDuplicateForMap(x, y, i));
            
            usedPositionsForMap[i][0] = x;
            usedPositionsForMap[i][1] = y;
            isPositionUsedForMap[i] = true;

            pSharedData->minerals[map_idx][i].pos.x = x;
            pSharedData->minerals[map_idx][i].pos.y = y;
            pSharedData->minerals[map_idx][i].map_id = map_id;
            pSharedData->minerals[map_idx][i].is_collected = false;
            
            // 맵별로 다른 광물 분포 설정
            if (map_id == -1) { // 좌측 맵 - 석탄이 많음
                pSharedData->minerals[map_idx][i].type = (rand() % 10 < 6) ? COAL : (MineralType)(rand() % 3);
            } else if (map_id == 0) { // 중앙 맵 - 균등 분포
                pSharedData->minerals[map_idx][i].type = (MineralType)(rand() % 3);
            } else { // 우측 맵 - 금이 많음
                pSharedData->minerals[map_idx][i].type = (rand() % 10 < 4) ? GOLD : (MineralType)(rand() % 3);
            }

            switch (pSharedData->minerals[map_idx][i].type) {
                case COAL:
                    pSharedData->minerals[map_idx][i].symbol = 'C';
                    break;
                case IRON:
                    pSharedData->minerals[map_idx][i].symbol = 'I';
                    break;
                case GOLD:
                    pSharedData->minerals[map_idx][i].symbol = 'G';
                    break;
            }
        }
    }

    for (int i = 0; i < MAX_PLAYERS; ++i) {
        pSharedData->players[i].is_active = false;
        pSharedData->players[i].current_map_id = 0; // 시작은 중앙 맵
    }

    std::cout << "MMF created and initialized with minerals and players." << std::endl;
    std::cout << "Press Enter to unmap the file and exit." << std::endl;
    std::cin.get();

    UnmapViewOfFile(pSharedData);
    CloseHandle(hMapFile);

    return 0;
}
