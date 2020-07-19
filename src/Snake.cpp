#include "Snake.hpp"

Snake::Snake( Snake::Direction direction, int x, int y, int length ) : direction{ direction },
                                                                       x{ x },
                                                                       y{ y },
                                                                       length{ length } {
    int xPos = x;
    int yPos = y;
    switch ( direction ) {
        case Direction::up: yPos += length - 1;
            break;
        case Direction::down: yPos -= length - 1;
            break;
        case Direction::right: xPos -= length - 1;
            break;
        case Direction::left: xPos += length - 1;
    }
    turns.push_back( { xPos, yPos } );
}

std::vector<std::pair<int, int>> Snake::GetFilledIn() {
    std::vector<std::pair<int, int>> spaces;
    spaces.reserve( length );

    Turn last{ x, y };
    for ( const auto& current : turns ) {
        const auto increment = current.x > last.x ? []( int& x_, int& y_ ) { ++x_; } :
                               current.x < last.x ? []( int& x_, int& y_ ) { --x_; } :
                               current.y > last.y ? []( int& x_, int& y_ ) { ++y_; } :
                               current.y < last.y ? []( int& x_, int& y_ ) { --y_; } :
                               []( int&, int& ) {};
        for ( int  x_        = last.x, y_ = last.y; x_ != current.x || y_ != current.y; increment( x_, y_ )) {
            if ( length == spaces.size()) break;
            spaces.emplace_back( x_, y_ );
        }
        if ( length == spaces.size()) break;
        last = current;
    }

    return spaces;
}

void Snake::Advance( unsigned short spaces ) {
    switch ( direction ) {
        case Direction::up: y -= spaces;
            break;
        case Direction::down: y += spaces;
            break;
        case Direction::right: x += spaces;
            break;
        case Direction::left: x -= spaces;
            break;
    }
    // TODO remove trailing turns
}

void Snake::Up() {
    if ( direction == Direction::up || direction == Direction::down ) return;
    turns.push_front( { x, y } );
    direction = Direction::up;
}

void Snake::Down() {
    if ( direction == Direction::up || direction == Direction::down ) return;
    turns.push_front( { x, y } );
    direction = Direction::down;
}

void Snake::Right() {
    if ( direction == Direction::right || direction == Direction::left ) return;
    turns.push_front( { x, y } );
    direction = Direction::right;
}

void Snake::Left() {
    if ( direction == Direction::right || direction == Direction::left ) return;
    turns.push_front( { x, y } );
    direction = Direction::left;
}

void Snake::Grow( unsigned short spaces ) {
    length += spaces;
}
