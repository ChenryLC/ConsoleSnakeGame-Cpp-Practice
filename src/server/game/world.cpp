#include <algorithm>
#include <memory>
#include "./manager/chunk_manager.h"
#include "./manager/snake_manager.h"
#include "world.h"
#include "../error/game_error.h"

bool World::setSnakeDirection(int id, Direction dir){
    return snakes.setDirection(id, dir);
}

std::vector<int> World::getSnakeList(){
    return snakes.getAllId();
}

std::vector<int> World::getAliveSnakeList(){
    return snakes.getAliveIds();
}

std::optional<Direction> World::getSnakeDirection(int id){
    return snakes.getDirection(id);
}

std::optional<SnakeStatus> World::getSnakeStatus(int id){
    auto s = snakes.getSnake(id);
    if(!s) return std::nullopt;
    return s.value().get().getStatus();
}

std::optional<std::reference_wrapper<const Snake::body_type>> 
    World::getSnakeBody(int id){
        auto s=snakes.getSnake(id);
        if(s){
            return std::optional(std::ref(s.value().get().getBody()));
        }else
            return std::nullopt;
}

std::optional<Position> World::getSnakeHeadPos(int id) const{
    return snakes.getHead(id);
}

size_t World::getStepCount() const{
    return step_count;
}

size_t World::getMapSize() const{
    return chunks.getMapSize();
}

std::optional<std::reference_wrapper<const std::unordered_set<LocalPos>>> 
    World::getSnakeChunk(ChunkPos c) const{
        return chunks.getSnakeChunk(c);
}

std::optional<std::reference_wrapper<const std::unordered_set<LocalPos>>> 
    World::getFoodChunk(ChunkPos c) const{
        return chunks.getFoodChunk(c);
}

const auto& World::getToSnake() const{
    return chunks.getPosToSnake();
}


const auto& World::getToFood() const{
    return chunks.getPosToFood();
}

const auto& World::getToAir() const{
    return chunks.getPosToAir();
}

const std::unordered_multiset<Position> World::getAllHeadPos() const{
    return snakes.getHeadPoses();
}


void World::step(){
    //头移动
    snakes.moveHeads();
    //食物检测
    std::vector<bool> has_eat;
    for(auto s : snakes.getHeadPoses()){
        
    }
    //尾移动
    //死亡逻辑
    //区块加载
    //事件回调（暂无）
}