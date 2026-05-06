#pragma once
#include <unordered_set>

struct Position{
    int x, y;
    Position(int x_,int y_):x(x_), y(y_){}
    Position():x(), y(){}
    bool operator== (const Position &pos) const {return (pos.x==x&&pos.y==y);}
    bool operator!= (const Position &pos) const {return (pos.x!=x||pos.y!=y);}
};

namespace std{
    template<>
    struct hash<Position> {
        size_t operator()(const Position& p) const noexcept {
            /*
            size_t h1 = hash<int>{}(p.x);
            size_t h2 = hash<int>{}(p.y);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            */
            return (hash<int>()(p.x) << 16) ^ hash<int>()(p.y);
        }
    };
}