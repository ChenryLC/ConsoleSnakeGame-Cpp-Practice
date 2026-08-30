#include "snake_manager.h"
#include "../../error/game_error.h"

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
    return (index.find(id) != index.end());
}

const std::unordered_multiset<Position>& SnakeManager::getHeadPoses() const{
    return head_pos_set;
}

const std::unordered_multiset<Position>& SnakeManager::getTailPoses() const{
    return tail_pos_set;
}

/// @brief clear all snake datas
void SnakeManager::clearAllSnakes(){
    snakes.clear();
    index.clear();
    head_pos_set.clear();
    tail_pos_set.clear();
    dirs.clear();
}

/// @brief clear datas of snakes which is marked dead
void SnakeManager::clearDeadSnakes(){
    std::vector<int> clear_list;
    clear_list.reserve(snakes.size());
    for(const auto& s : snakes){
        if(s.getStatus()==SnakeStatus::Dead){
            clear_list.push_back(s.getId());
        }
    }
    
    bool to_rebuild_pos_to_id = false;
    if(clear_list.size() >= snakes.size()/2){//if amount of dead snake is too big
        to_rebuild_pos_to_id = true;
    }
    for(auto id : clear_list){
        size_t i = getIndex(id);
        index[snakes.back().getId()]=i;
        index.erase(id);
        if(!to_rebuild_pos_to_id){
            for(auto& p : snakes[i].getBody()){
            pos_to_id.erase(p);
            }
        }
        using std::swap;
        if(i!=snakes.size()-1)
        swap(snakes[i], snakes.back());
        snakes.pop_back();
        dirs.erase(id);
    }
    if(to_rebuild_pos_to_id){
        pos_to_id.clear();
            for(auto& s : snakes){
                int id = s.getId();
                for(auto p : s.getBody()){
                    pos_to_id.emplace(p, id);
                }
            }
            
    }
    refreshPosSet();
}

/// @brief refresh the datas in head_pos_set, tail_pos_set and head_to_id
void SnakeManager::refreshPosSet(){
    head_pos_set.clear();
    tail_pos_set.clear();

    head_to_id.clear();
    for (const auto& s : snakes) {
        head_pos_set.insert(s.getHead());
        tail_pos_set.insert(s.getTail());
        head_to_id.emplace(s.getHead(), s.getId());
    }
}

/// @brief remove snake of given id
/// @param id id of snake to remove
/// @return return true if snake is found and cleared, false for id is not found 
bool SnakeManager::removeSnake(int id){
    if(index.find(id)==index.end())return 0;
    size_t snake_index = getIndex(id);
    for(auto p : snakes[snake_index].getBody()){
        pos_to_id.erase(p);
    }
    head_to_id.erase(snakes[snake_index].getHead());
    index[snakes.back().getId()]=snake_index;//set index of last of the "snake" into the index of snake to be removed
    index.erase(id);//erase snake ready to be erase
    using std::swap;
    //swap and pop from snakes
    swap(snakes.back(), snakes[snake_index]);
    snakes.pop_back();
    dirs.erase(id);//clear direction data
    refreshPosSet();
    return true;
}

bool SnakeManager::addSnake(int id, Snake::body_type& body, Direction dir){
    if(index.find(id)!=index.end())return false;
    //add information to datas
    index.emplace(id, snakes.size());
    snakes.emplace(snakes.end(), body, id, dir);
    dirs.emplace(id, dir);
    for(auto p : body){//add body to pos-id index
        pos_to_id.emplace(p, id);
    }
    head_to_id.emplace(body.front(), id);//add head pos-id index
    refreshPosSet();
    return true;
}

bool SnakeManager::resetSnake(int id, Snake::body_type& body, Direction dir){
    if(index.find(id)==index.end())return false;
    auto snake_index = getIndex(id);
    for(auto p : snakes[snake_index].getBody()){//clear old data in pos_to_id
        pos_to_id.erase(p);
    }
    head_to_id.erase(snakes[snake_index].getHead());//clear old data
    snakes[snake_index].setBody(body);
    dirs[id]=dir;
    for(auto p : body){//emplace new one to it
        pos_to_id.emplace(p, id);
    }
    head_to_id.emplace(body.front(), id);//emplace new one
    refreshPosSet();
    return true;
}

void SnakeManager::moveHeads(){
    for(auto& s : snakes){
        auto old_pos = s.getHead();
        auto p = s.moveHead(dirs.at(s.getId()));
        if(p){
            pos_to_id.erase(old_pos);
            pos_to_id.emplace(p.value(), s.getId());
        }
    }
    refreshPosSet();
}

void SnakeManager::moveTails(const std::unordered_map<int, bool>& isEat){
    for(auto& s : snakes){
        Position old_pos = s.getTail();
        if(!isEat.at(s.getId())){
            pos_to_id.erase(old_pos);
        }
        s.moveTail(isEat.at(s.getId()));
    }
    refreshPosSet();
}

SnakeManager::SnakeManager(std::vector<int> id_list, std::vector<Snake> snake_list, size_t max_length): snakes(snake_list), snake_max_length(max_length) {
    if(id_list.size()!=snake_list.size())
        throw error::InitializationException("snake_list and id_list must have same size");
    
    for(size_t i=0; i!=id_list.size(); ++i){
        index.emplace(id_list[i], i);
        head_pos_set.emplace(snake_list[i].getHead());
        tail_pos_set.emplace(snake_list[i].getTail());
        head_to_id.emplace(snake_list[i].getHead(), id_list[i]);
        for(auto p : snake_list[i].getBody()){
            pos_to_id.emplace(p, id_list[i]);
        }
    }
}

/// @brief get id of snake in the given pos. return nullopt if no snake on it
/// @param p the positon to request
/// @return an int for id wrapped in std::optional if a valid body part in the pos, std::nullopt if it's invalid
std::optional<int> SnakeManager::getIdByPos(Position p)const{
    auto it = pos_to_id.find(p);
    if(it == pos_to_id.end()) return std::nullopt;
    return it->second;//return the "second" data member(player id) from the "it" pointed pair
} 
