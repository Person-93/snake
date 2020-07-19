#pragma once

#include <deque>
#include <vector>
#include <utility>

class Snake {
public:
    enum class Direction { up, down, right, left };

    Snake( Direction direction, int x, int y, int length );

    std::vector<std::pair<int, int>> GetFilledIn();

    void Advance( unsigned short spaces );

    void Up();

    void Down();

    void Right();

    void Left();

    void Grow( unsigned short spaces );

private:
    Direction direction;
    int       x;
    int       y;
    int       length;

    struct Turn {
        int x;
        int y;
    };

    std::deque<Turn> turns;
};



