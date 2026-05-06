#pragma once
#include <vector>
#include <unordered_set>
#include <random>
#include <memory>
#include "constant.h"
#include "snake.h"
#include "chunk.h"
#include "../error/game_error.h"
#include "../event/game_event.h"

class World{
    private:
    
    unsigned map_size;//以区块计
    std::vector<int> idList;
    std::vector<Snake> snakeList;
    std::unordered_map<ChunkPos,Chunk> chunkList;
    std::vector<ChunkPos> chunkUpdateList;
    std::unordered_map<int, Direction> playerDirection;
    std::unordered_map<int, ChunkPos> chunkPosOfPlayer;

    std::vector<ChunkPos> dirtyChunkList;

    std::mt19937 random_engine;

    std::shared_ptr<IGameEventHandler> game_event_handler;

    size_t step_count=0;
    
    int playerJoin(int playerId_);
    void addFood(ChunkPos);
    void chunkUpdate();

    public:

    World(int sz_, std::vector<int> playerList, std::shared_ptr<IGameEventHandler> =nullptr);


    void step();
    bool playerLeave(int playerId_);

    int getMapSize() const{return map_size*CHUNK_SIZE;}
    size_t getCurrentStep() const{return step_count;}

    const Position& getPlayerHeadPos(int) const;
    const Chunk& getChunk(const ChunkPos&) const;
    const std::optional<Snake> getSnake(int) const;
    const std::vector<ChunkPos>& getDirtyChunks() const{return dirtyChunkList;}
    std::vector<ChunkPos> getLoadedChunkPos() const;

    std::vector<int> getActivePlayerIdList() const;
    std::vector<int> getAllPlayerIdList() const;
    bool isPlayerAlive(int) const;

    bool setPlayerDirection(int, Direction);
    std::unordered_map<int, bool> setMultiplePlayerDirection(const std::unordered_map<int, Direction>);
};