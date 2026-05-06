#include "snake.h"

std::optional<Position> Snake::moveHead(Direction dir){
    if ((dir == Direction::Up    && current_dir == Direction::Down) ||
    (dir == Direction::Down  && current_dir == Direction::Up) ||
    (dir == Direction::Left  && current_dir == Direction::Right) ||
    (dir == Direction::Right && current_dir == Direction::Left)){//禁止180°调头
        dir=current_dir;
    }
        std::optional<Position> newHead;
        switch(dir){
            case Direction::Up:
                newHead={body.front().x,body.front().y+1};
                break;
            case Direction::Down:
                newHead={body.front().x,body.front().y-1};
                break;
            case Direction::Left:
                newHead={body.front().x-1,body.front().y};
                break;
            case Direction::Right:
                newHead={body.front().x+1,body.front().y};
                break;
            case Direction::Null:
                moved = false;
                return std::nullopt;
        }
        moved = true;
        body.push_front(newHead.value());
        current_dir = dir;
        return newHead;
}

std::optional<Position> Snake::moveTail(bool isEat){
    if (!moved || isEat){
        moved=false;
        return std::nullopt;
    }
    moved=false;
    std::optional<Position> pos = body.back();
    body.pop_back();
    return pos;
}

Snake::Snake(std::deque<Position> body_, int id_, Direction dir_):body(body_), id(id_), current_dir(dir_), status(SnakeStatus::Alive), moved(false){}

const SnakeStatus& Snake::getStatus() const{
    return status;
}

const int& Snake::getId() const{
    return id;
}

const Position& Snake::getTail() const{
    return body.back();
}

const Position& Snake::getHead() const{
    return body.front();
}

const std::deque<Position> &Snake::getBody() const{
    return body;
}

void Snake::setStatus(SnakeStatus sta){
    status = sta;
}

void Snake::swap(Snake &other)noexcept{
    using std::swap;
    swap(current_dir, other.current_dir);
    swap(body, other.body);
    swap(id, other.id);
    swap(status, other.status);
    swap(moved, other.moved);
}

void swap(Snake &a, Snake &b)noexcept{
    a.swap(b);
}

