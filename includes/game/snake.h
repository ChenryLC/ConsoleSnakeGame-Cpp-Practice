#pragma once

#include <deque>
#include <optional>
#include <functional>
#include "position.h"

enum class Direction{
    Null, Up, Left, Right, Down
};

enum class SnakeStatus{
    Null, Alive, Dead, Win
};

class Snake{
    private:    
        std::deque<Position> body;
        Direction current_dir;
        int id;
        SnakeStatus status;
    public:

        Snake(std::deque<Position>, int, Direction);
        
        std::pair<std::optional<Position>, std::optional<Position>> move(bool, Direction);
        const Position& getHead() const;
        const Position& getTail() const;
        const SnakeStatus& getStatus() const;
        const int& getId() const;
};

namespace std{
    template<>
    struct hash<Snake> {
        size_t operator()(const Snake& s) const noexcept {
            return s.getId();
        }
    };
}