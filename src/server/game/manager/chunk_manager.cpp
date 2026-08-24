#include "chunk_manager.h"

ChunkManager::ChunkManager(int map_size_, int load_radius_,
    std::mt19937& random_gen_)
        :world_size(map_size_ & ~15),world_size_by_chunk(world_size/16),
         random_gen(random_gen_), random_range(0, 15)
{}

bool ChunkManager::hasChunk(ChunkPos p) const{
    return load_level.find(p) != load_level.end();
}

bool ChunkManager::hasFood(Position p) const{
    if (!hasChunk(getChunkPos(p))) return false;
    auto fp = food_chunks.at(getChunkPos(p));
    return fp.find(toLocalPos(p)) != fp.end();
}

bool ChunkManager::hasSnake(Position p) const{
    if (!hasChunk(getChunkPos(p))) return false;
    auto sp = snake_chunks.at(getChunkPos(p));
    return sp.find(toLocalPos(p)) != sp.end();
}

bool ChunkManager::addFood(ChunkPos p){
    if (!hasChunk(p)) return false;
    LocalPos pos{random_range(random_gen), random_range(random_gen)};
    pos_to_food.push_back(toWorldPos(pos, p));
    return
    food_chunks.at(p).insert(pos)
            .second;
}

bool ChunkManager::popFood(Position p){
    if (!hasChunk(getChunkPos(p))) return false;
    pos_to_air.push_back(p);
    return
    food_chunks.at(getChunkPos(p)).erase(toLocalPos(p))/* ==1 */; 
}

bool ChunkManager::insertSnake(Position p){
    if (!hasChunk(getChunkPos(p))) return false;
    pos_to_snake.push_back(p);
    return
    snake_chunks.at(getChunkPos(p)).emplace(toLocalPos(p)).second;
}

bool ChunkManager::popSnake(Position p){
    if (!hasChunk(getChunkPos(p))) return false;
    pos_to_air.push_back(p);
    return
    snake_chunks.at(getChunkPos(p)).erase(toLocalPos(p));
}

bool ChunkManager::loadChunk(ChunkPos c){
    if(c.x>world_size_by_chunk||c.y>world_size_by_chunk) return false;
    if(load_level.find(c)!=load_level.end()) return false;
    snake_chunks.emplace(c, std::unordered_set<LocalPos>());//add empty chunkta
    food_chunks.emplace(c, std::unordered_set<LocalPos>());
    load_level.emplace(c, 0);
    return true;
}

bool ChunkManager::increaseLoadLevel(ChunkPos c){
    if(load_level.find(c)==load_level.end()) return false;
    ++load_level.at(c);
    return true;
}

bool ChunkManager::decreaseLoadLevel(ChunkPos c){
    if(load_level.find(c)==load_level.end()) return false;
    --load_level.at(c);
    if(load_level[c]==0) unload_list.add(c);
    return true;
}

//ChunkManager::DelayUnloadList::DelayUnloadList(size_t buffer_size)
//    :capacity_(buffer_size), buffer_(buffer_size), unload_list_(buffer_size) 
//        , head_index_(0), tail_index_(0), count_(0)
//        , need_unload(false)
//{}

void ChunkManager::DelayUnloadList::add(const ChunkPos c){
    if(count_==capacity_){
        need_unload = true;
        size_t move_count = capacity_/2;
        for (size_t i=0; i!=move_count; ++i){
            unload_list_.push_back(buffer_[(head_index_+i)%capacity_]);
        }
        count_ -= move_count;
        head_index_ = (head_index_+move_count)%capacity_;
    }
    if(count_==0){
        head_index_=0;
        tail_index_=0;
    }
    buffer_[tail_index_]=c;
    tail_index_ = (++tail_index_)%capacity_;
    ++count_;
    return;
}

void ChunkManager::DelayUnloadList::clear(){
    unload_list_.clear();
    need_unload=false;
    head_index_=0;
    tail_index_=0;
    count_=0;
    return;
}

void ChunkManager::DelayUnloadList::clear(size_t count){
    if(count>=count_){
        clear();
        return;
    }
    head_index_=(head_index_+count)%capacity_;
    count_ -= count;
    return;
}

bool ChunkManager::DelayUnloadList::empty() const{
    if(count_==0)return true;
    return false;
}

bool ChunkManager::DelayUnloadList::full() const{
    if(count_==capacity_)return true;
    return false;
}

size_t ChunkManager::DelayUnloadList::size() const{
    return count_;
}

size_t ChunkManager::DelayUnloadList::capacity() const{
    return capacity_;
}

std::vector<ChunkPos>& ChunkManager::DelayUnloadList::getUnloadList(){
    return unload_list_;
}

void ChunkManager::flushUnload(){
    auto& list = unload_list.getUnloadList();
    if(!unload_list.need_unload)return;
    int i = 0;
    int removed_count=0;
    while(i!=list.size()&&removed_count!=10){
        const ChunkPos& pos = list[i];
        auto it = load_level.find(pos);
        if(it==load_level.end()||it->second<=0){
            snake_chunks.erase(list[i]);
            food_chunks.erase(list[i]);
            load_level.erase(list[i]);
            list[i]=list.back();
            list.pop_back();
            ++removed_count;
        }else{
            ++i;
        }
    }
    if(list.empty()){
        unload_list.need_unload=false;
    }
    
    return;
}

void ChunkManager::clear(){
    snake_chunks.clear();
    food_chunks.clear();
    load_level.clear();
    unload_list.clear();
}

auto ChunkManager::getPosToAir() const{
    return pos_to_air;
}

auto ChunkManager::getPosToFood() const{
    return pos_to_food;
}

auto ChunkManager::getPosToSnake() const{
    return pos_to_snake;
}

void ChunkManager::clearChangeRecord(){
    pos_to_air.clear();
    pos_to_food.clear();
    pos_to_snake.clear();
}