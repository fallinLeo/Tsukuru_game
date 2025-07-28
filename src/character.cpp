#include "../header/character.h"
#include "../header/header.h"

void draw_object(const Position& pos, const Object& obj) {
    char buffer[2] = { obj.symbol, '\0' };
    print_char(pos.x, pos.y, buffer);
}
void draw_player(const Player& player) {
    print_char(player.pos.x, player.pos.y, player.symbol); // player.symbol은 문자열
}