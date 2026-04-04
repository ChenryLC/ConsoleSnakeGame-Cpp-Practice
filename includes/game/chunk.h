#include <unordered_set>
#include <algorithm>
#include "position.h"
#include "constant.h"

struct Chunk{
    std::unordered_set<Position> snake;
    std::unordered_set<Position> food;
    int load_level;
    Chunk():Chunk(0){}
    Chunk(int load_level_):load_level(load_level_){}
};

int getChunkFoodCount(Chunk c){
    return c.food.size();
}
//



struct ChunkPos{
    int x, y;
    ChunkPos(int x_,int y_):x(x_), y(y_){}
    ChunkPos():x(), y(){}
    bool operator== (const Position &pos) const {return (pos.x==x&&pos.y==y);}
    bool operator!= (const Position &pos) const {return (pos.x!=x||pos.y!=y);}
};

namespace std{
    template<>
    struct hash<ChunkPos> {
        size_t operator()(const ChunkPos& p) const noexcept {
            return (hash<int>()(p.x) << 16) ^ hash<int>()(p.y);
        }
    };
}

inline ChunkPos getChunkPos(const Position &pos){
    return ChunkPos(pos.x/CHUNK_SIZE, pos.y/CHUNK_SIZE);
}

inline Position getPosInChunk(const Position &pos){
    return Position(pos.x%CHUNK_SIZE, pos.y%CHUNK_SIZE);
}