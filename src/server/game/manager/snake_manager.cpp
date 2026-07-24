#include "snake_manager.h"

size_t SnakeManager::getIndex(int id) const{
    return index.at(id);
}

std::optional<std::reference_wrapper<const Snake>> SnakeManager::getSnake(int id) const{
    if(index.find(id)==index.end()) return std::nullopt;
    return std::cref(snakes[getIndex(id)]);
}

std::optional<Position> SnakeManager::getHead(int id) const{
    if(index.find(id)==index.end()) return std::nullopt;
    return snakes[getIndex(id)].getHead();
}

std::optional<Position> SnakeManager::getTail(int id) const{
    if(index.find(id)==index.end()) return std::nullopt;
    return snakes[getIndex(id)].getTail();
}

int SnakeManager::getSankeCount() const{
    return snakes.size();
}

std::vector<int> SnakeManager::getAllId() const{
    std::vector<int> id_list;
    for(auto [id, i] : index){
        id_list.push_back(id);
    }
    return id_list;
}

std::vector<int> SnakeManager::getAliveIds() const {
    std::vector<int> id_list;
    for (auto [id, i] : index){
        if(snakes[i].getStatus()==SnakeStatus::Alive){
            id_list.push_back(id);
        }
    }
    return id_list;
}

std::optional<Direction> SnakeManager::getDirection(int id) const{
    if(dirs.find(id)==dirs.end())return std::nullopt;
    return dirs.at(id);
}

std::optional<bool> SnakeManager::isSelfColliding(int id) const{
    if(index.find(id)==index.end()) return std::nullopt;
    auto& snake = snakes[getIndex(id)];
    auto& snakeHead = snake.getHead();
    bool isColliding = false;
    auto& body = snake.getBody();
    for (auto it = body.begin()+1;it!=body.end();++it){
        if(*it==snakeHead){
            isColliding = true;
            break; 
        }
    }
    return isColliding;
}

bool SnakeManager::isAlive(int id) const{
    return snakes[getIndex(id)].getStatus()==SnakeStatus::Alive;
}

bool SnakeManager::setDead(int id){
    auto it = index.find(id);
    if (it == index.end()){
        return false;
    }
    auto i = (*it).second;
    snakes[i].setStatus(SnakeStatus::Dead);
    return true;
}

bool SnakeManager::setDirection(int id, Direction dir){
    auto it = index.find(id);
    if (it == index.end()){
        return false;
    }
    auto i = (*it).second;
    dirs[i] = dir;
    return true;
}

void SnakeManager::setSnakeMaxLength(int length){
    snake_max_length = length;
}

bool SnakeManager::hasId(int id) const{
    return (index.find(id) == index.end());
}

const std::unordered_multiset<Position>& SnakeManager::getHeadPoses() const{
    return head_pos_set;
}

const std::unordered_multiset<Position>& SnakeManager::getTailPoses() const{
    return tail_pos_set;
}

void SnakeManager::clearAllSnakes(){
    snakes.clear();
    index.clear();
    head_pos_set.clear();
    tail_pos_set.clear();
    dirs.clear();
}


void SnakeManager::clearDeadSnakes(){
    std::vector<int> clear_list;
    clear_list.reserve(snakes.size());
    for(const auto& s : snakes){
        if(s.getStatus()==SnakeStatus::Dead){
            clear_list.push_back(s.getId());
        }
    }
    for(auto id : clear_list){
        size_t i = getIndex(id);
        index[snakes.back().getId()]=i;
        index.erase(id);
        using std::swap;
        if(i!=snakes.size()-1)
        swap(snakes[i], snakes.back());
        snakes.pop_back();
        dirs.erase(id);
    }
    refreshPosSet();
}

void SnakeManager::refreshPosSet(){
    head_pos_set.clear();
    tail_pos_set.clear();
    for (const auto& s : snakes) {
        head_pos_set.insert(s.getHead());
        tail_pos_set.insert(s.getTail());
    }
}

bool SnakeManager::removeSnake(int id){
    if(index.find(id)==index.end())return 0;
    size_t snake_index = getIndex(id);
    index[snakes.back().getId()]=snake_index;
    index.erase(id);
    using std::swap;
    swap(snakes.back(), snakes[snake_index]);
    snakes.pop_back();
    dirs.erase(id);
    refreshPosSet();
    return true;
}

bool SnakeManager::addSnake(int id, Snake::body_type& body, Direction dir){
    if(index.find(id)!=index.end())return false;
    index.emplace(id, snakes.size());
    snakes.emplace(snakes.end(), body, id, dir);
    dirs.emplace(id, dir);
    refreshPosSet();
    return true;
}

bool SnakeManager::resetSnake(int id, Snake::body_type& body, Direction dir){
    if(index.find(id)==index.end())return false;
    snakes[getIndex(id)].setBody(body);
    dirs[id]=dir;
    return true;
}

void SnakeManager::moveHeads(){
    for(auto& s : snakes){
        auto p = s.moveHead(dirs.at(s.getId()));
        if(p){
            head_pos_to_id.erase(s.getHead());
            head_pos_to_id.emplace(p.value(), s.getId());
        }
    }
    refreshPosSet();
}

void SnakeManager::moveTails(const std::unordered_map<int, bool>& isEat){
    for(auto& s : snakes){
        s.moveTail(isEat.at(s.getId()));
    }
    refreshPosSet();
}


