#include "../header/character.h"
#include "../header/header.h"
#include "../header/item.h"

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