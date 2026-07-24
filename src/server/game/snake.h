#pragma once

#include <deque>
#include <optional>
#include <functional>
#include "../../common/direction.h"
#include "../../common/position.h"

enum class SnakeStatus{
    Null, Alive, Dead
};

class Snake{
    public:   
        using body_type = std::deque<Position>;
    private:
        body_type body;
        Direction current_dir;
        int id;
        SnakeStatus status;
        bool moved;
    public:



        bool operator==(const Snake &s){return id==s.id;}

        Snake(std::deque<Position>, int, Direction = Direction::Null);
        
        std::optional<Position> moveHead(Direction);
        std::optional<Position> moveTail(bool);
        const Position& getHead() const;
        const Position& getTail() const;
        const body_type &getBody() const;
        const SnakeStatus& getStatus() const;
        void setStatus(SnakeStatus);
        const int& getId() const;
        void swap(Snake&) noexcept;
        void setBody(body_type& body);
};

void swap(Snake &a, Snake &b)noexcept;

namespace std{
    template<>
    struct hash<Snake> {
        size_t operator()(const Snake& s) const noexcept {
            return s.getId();
        }
    };
}