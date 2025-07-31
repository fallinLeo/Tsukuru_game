#include "../header/character.h"
#include "../header/header.h"
#include "../header/item.h"
#include <limits>
#include <iostream>

void draw_object(const Position& pos, const Object& obj) {
    char buffer[2] = { obj.symbol, '\0' };
    print_char(pos.x, pos.y, buffer);
}
void draw_player(const Player& player) {
    print_char(player.pos.x, player.pos.y, player.symbol); // player.symbol은 문자열
}

void decreaseLife(Player& p, int player_idx, int amount = 1) {
    if (p.life > 0) {
        p.life--;
    }
    else p.gameover(player_idx);
}

bool isNearMineral(Position& p, Mineral& mineral) {
    bool res = false;
    if ((abs(p.x - mineral.pos.x) <= 1 && p.pos.y == mineral.pos.y) || (abs(p.pos.y - mineral.pos.y) <= 1 && p.pos.x == mineral.pos.x)){
        res = true;
    }
    return res;
}


// AI관련 코드
// GOLD를 찾는 함수 구현
Mineral* findNearestGold(SharedData* data, int mapId, int x, int y) {
    Mineral* nearest = nullptr;
    int minDist = std::numeric_limits<int>::max();
    for (int i = 0; i < MAX_MINERALS; ++i) {
        Mineral& m = data->minerals[get_map_index_by_id(mapId)][i];
        if (!m.is_collected && m.type == GOLD && m.map_id == mapId) {
            int dist = abs(m.pos.x - x) + abs(m.pos.y - y);
            if (dist < minDist) {
                minDist = dist;
                nearest = &m;
            }
        }
    }
    return nearest;
}
// AI 행동 함수
void updateAI(SharedData* data) {
    Player* ai = &data->players[AI_PLAYER_ID];
    if (!ai->is_active) return;

    int mapId = ai->current_map_id;
    int x = ai->pos.x;
    int y = ai->pos.y;

    Mineral* target = findNearestGold(data, mapId, x, y);
    if (target == nullptr) return;

    // 방향 결정
    if (target->pos.x > x) x++;
    else if (target->pos.x < x) x--;
    else if (target->pos.y > y) y++;
    else if (target->pos.y < y) y--;

    ai->pos.x = x;
    ai->pos.y = y;

    // GOLD 수집
    if (x == target->pos.x && y == target->pos.y) {
        target->is_collected = true;
        data->minerals_collected++;
        std::cout << "AI: GOLD collected at (" << x << ", " << y << ")" << std::endl;
    }
}