#pragma once
#include <unordered_set>
#include <algorithm>
#include "position.h"
#include "constant.h"
#include "local_pos.h"
struct ChunkPos{
    int x, y;
    ChunkPos(int x_,int y_):x(x_), y(y_){}
    ChunkPos():x(), y(){}
    bool operator== (const ChunkPos &pos) const {return (pos.x==x&&pos.y==y);}
    bool operator!= (const ChunkPos &pos) const {return (pos.x!=x||pos.y!=y);}
    bool operator< (const ChunkPos &pos) const {if(x!=pos.x)return x<pos.x; return y<pos.y;}
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

inline Position toWorldPos(const LocalPos &lpos, const ChunkPos &c){
    return Position(lpos.x+c.x*CHUNK_SIZE, lpos.y+c.y*CHUNK_SIZE);
}

inline LocalPos toLocalPos(const Position &wpos){
    return LocalPos(wpos.x%CHUNK_SIZE, wpos.y%CHUNK_SIZE);
}