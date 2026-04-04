#pragma once
#include <vector>
#include <unordered_set>
#include <random>
#include "snake.h"
#include "chunk.h"
#include "constant.h"
#include "../error/game_error.h"

class World{
    private:;
    const int world_size;
    std::unordered_set<int> idList;
    std::vector<Snake> snakeList;
    std::unordered_map<ChunkPos, Chunk> chunkMap;
    std::unordered_set<Position> snakePos;
    std::unordered_set<Position> foodPos;

    std::mt19937 random_engine;

    ChunkPos unloadChunk(ChunkPos);
    ChunkPos loadChunk(ChunkPos);
    bool updateChunkInfluence(ChunkPos);
    bool applyChunkStatus();
    bool chunkStatusUpdate();
    void addFood();

    int playerJoin(int playerId_);

    public:

    World(int sz_, std::vector<int> playerList);

    

    void step();
    int playerLeave(int playerId_);
    const Chunk& getChunk(ChunkPos) const;
};