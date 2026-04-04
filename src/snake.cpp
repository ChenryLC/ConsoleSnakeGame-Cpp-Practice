#include "snake.h"

std::pair<std::optional<Position>, std::optional<Position>>//返回｛新头坐标, 新尾坐标｝
 Snake::move(bool isEat, Direction dir){
    if ((dir == Direction::Up    && current_dir == Direction::Down) ||
    (dir == Direction::Down  && current_dir == Direction::Up) ||
    (dir == Direction::Left  && current_dir == Direction::Right) ||
    (dir == Direction::Right && current_dir == Direction::Left)){//禁止180°调头
        return {std::nullopt, std::nullopt};
    }else{
        std::optional<Position> oldTail= body.back();
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
                return {std::nullopt, std::nullopt};    
        }
        body.push_front(newHead.value());
        current_dir = dir;
        if(!isEat){
            body.pop_back();
        }else{
            oldTail = std::nullopt;
        }
        return {newHead, oldTail};
    }
}

Snake::Snake(std::deque<Position> body_, int id_, Direction dir_ = Direction::Null):body(body_), id(id_), current_dir(dir_), status(SnakeStatus::Alive){}

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