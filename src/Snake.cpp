#include "Snake.hpp"
#include "logging.hpp"

DEFINE_LOGGER( Snake );// NOLINT(cert-err58-cpp)

bool operator==( const Snake::Turn& a, const Snake::Turn& b ) {
    return a.x == b.x && a.y == b.y;
}

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

Snake::Response Snake::Up() {
    if ( direction == Direction::up || direction == Direction::down )
        return Response::ignore;
    if ( turns.front() == Turn{ x, y } ) {
        LOG( debug ) << "Deferred UP command";
        return Response::defer;
    }
    turns.push_front( { x, y } );
    direction = Direction::up;
    return Response::process;
}

Snake::Response Snake::Down() {
    if ( direction == Direction::up || direction == Direction::down )
        return Response::ignore;
    if ( turns.front() == Turn{ x, y } ) {
        LOG( debug ) << "Deferred DOWN command";
        return Response::defer;
    }
    turns.push_front( { x, y } );
    direction = Direction::down;
    return Response::process;
}

Snake::Response Snake::Right() {
    if ( direction == Direction::right || direction == Direction::left )
        return Response::ignore;
    if ( turns.front() == Turn{ x, y } ) {
        LOG( debug ) << "Deferred RIGHT command";
        return Response::defer;
    }
    turns.push_front( { x, y } );
    direction = Direction::right;
    return Response::process;
}

Snake::Response Snake::Left() {
    if ( direction == Direction::right || direction == Direction::left )
        return Response::ignore;
    if ( turns.front() == Turn{ x, y } ) {
        LOG( debug ) << "Deferred LEFT command";
        return Response::defer;
    }
    turns.push_front( { x, y } );
    direction = Direction::left;
    return Response::process;
}

void Snake::Grow( unsigned short spaces ) {
    length += spaces;
}
