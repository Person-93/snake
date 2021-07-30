#pragma once

#include <deque>
#include <vector>
#include <utility>

class Snake {
public:
    enum class Direction { up, down, right, left };

    enum class Response { ignore, process, defer };

    Snake( Direction direction, int x, int y, int length );

    std::vector<std::pair<int, int>> GetFilledIn();

    void Advance( unsigned short spaces );

    Response Up();

    Response Down();

    Response Right();

    Response Left();

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

    friend bool operator==( const Snake::Turn& a, const Snake::Turn& b );

    std::deque<Turn> turns;
};



