#pragma once
#include "../event/game_event.h"
#include "snake.h"
#include "../../common/chunk.h"
#include "manager/snake_manager.h"
#include "manager/chunk_manager.h"
#include <vector>
#include <unordered_set>
#include <random>
#include <functional>

class SnakeManager;
class ChunkManager;
class World{
private:
    SnakeManager snakes;
    ChunkManager chunks;

    size_t step_count = 0;

    void initialize();
    
public:
    World(int size, std::vector<int> players);

    void step();

    bool setSnakeDirection(int id, Direction dir);

    std::vector<int> getSnakeList();
    std::vector<int> getAliveSnakeList();
    std::optional<Direction> getSnakeDirection(int id);
    std::optional<SnakeStatus> getSnakeStatus(int id);
    std::optional<std::reference_wrapper<const Snake::body_type>> getSnakeBody(int id);
    std::optional<Position> getSnakeHeadPos(int id) const;

    size_t getStepCount() const;
    size_t getMapSize() const;
    std::optional<std::reference_wrapper<const std::unordered_set<LocalPos>>> getSnakeChunk(ChunkPos) const;
    std::optional<std::reference_wrapper<const std::unordered_set<LocalPos>>> getFoodChunk(ChunkPos) const;

    const auto& getToSnake() const;
    const auto& getToFood() const;
    const auto& getToAir() const;

    const std::unordered_multiset<Position> getAllHeadPos() const;
};