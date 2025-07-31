#pragma once
#include <iostream>
#include <windows.h>

struct Position{
    int x;
    int y;
};

class Object{
public:
    char symbol;
    std::string color;
    bool is_Obstacle;
    Position pos;
private:

};


class Player: public Object{
public:
    const char* symbol; // char → const char*로 변경
    bool is_active;
    int symbol_length;
    int life = 10;


    Player(const char* symbol, const std::string& color, bool is_Obstacle, const Position& pos,bool is_active) : Object(){
        this->symbol = symbol;
        this->color = color;
        this->is_Obstacle = is_Obstacle;
        this->is_active = is_active;
        this->pos = pos;
        this->symbol_length = strlen(symbol);
        if(strlen(symbol)==3){
            this->pos.x--; //타점위치 이동
            //this->pos.y++;
        }
    }

    void gameover(int player_idx) {
        this->is_active = false;
        UnmapViewOfFile(pSharedData);
        CloseHandle(hMapFile);
        release_screen();
        std::cout << "플레이어 " << plyaer_idx << "사망! " << std::endl;
    }
};



void draw_object(const Position& pos, const Object& obj);
void draw_player(const Player& player);
void decreaseLife(Player& p, int amount = 1);
bool isNearMineral(Position& p, Mineral& mineral);