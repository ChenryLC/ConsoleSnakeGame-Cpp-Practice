#pragma once
#include <unordered_set>
#include "constant.h"
#include "position.h"

struct ChunkPos;
struct LocalPos{
    int x, y;
    LocalPos(int x_,int y_):x(x_), y(y_){}
    LocalPos():x(), y(){}
    bool operator== (const LocalPos &pos) const {return (pos.x==x&&pos.y==y);}
    bool operator!= (const LocalPos &pos) const {return !(*this==pos);}
};

namespace std{
    template<>
    struct hash<LocalPos> {
        size_t operator()(const LocalPos& p) const noexcept {
            return (hash<int>()(p.x) << 16) ^ hash<int>()(p.y);
        }
    };
}

Position toWorldPos(const LocalPos &lpos, const ChunkPos &c);
LocalPos toLocalPos(const Position &wpos);