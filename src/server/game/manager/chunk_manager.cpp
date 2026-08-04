#include "chunk_manager.h"

ChunkManager::ChunkManager(int map_size_, int load_radius_,
    std::mt19937& random_gen_)
        :world_size(map_size_), load_radius(load_radius_),
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
    return
    food_chunks.at(p).
        emplace(random_range(random_gen), random_range(random_gen))
            .second;
}

bool ChunkManager::popFood(Position p){
    if (!hasChunk(getChunkPos(p))) return false;
    return
    food_chunks.at(getChunkPos(p)).erase(toLocalPos(p))/* ==1 */; 
}

bool ChunkManager::insertSnake(Position p){
    if (!hasChunk(getChunkPos(p))) return false;
    return
    snake_chunks.at(getChunkPos(p)).emplace(p).second;
}

bool ChunkManager::popSnake(Position p){
    if (!hasChunk(getChunkPos(p))) return false;
    return
    food_chunks.at(getChunkPos(p)).erase(toLocalPos(p));
}

