#pragma once
#include <unordered_set>
#include "chunk.h"
#include "constant.h"
#include "position.h"

struct LocalPos{
    int x, y;
    LocalPos(int x_,int y_):x(x_), y(y_){}
    LocalPos():x(), y(){}
    bool operator== (const LocalPos &pos) const {return (pos.x==x&&pos.y==y);}
    bool operator!= (const LocalPos &pos) const {return !(pos.x==pos.y);}
};

namespace std{
    template<>
    struct hash<LocalPos> {
        size_t operator()(const LocalPos& p) const noexcept {
            return (hash<int>()(p.x) << 16) ^ hash<int>()(p.y);
        }
    };
}

Position toWorldPos(const LocalPos &lpos, const ChunkPos &c){
    return Position(lpos.x+c.x*CHUNK_SIZE, lpos.y+c.y*CHUNK_SIZE);
}

LocalPos toLocalPos(const Position &wpos){
    return LocalPos(wpos.x%CHUNK_SIZE, wpos.y%CHUNK_SIZE);
}