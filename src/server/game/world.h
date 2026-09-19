#pragma once
#include "../event/game_event.h"
#include "snake.h"
#include <vector>
#include <unordered_set>
#include <random>
#include <memory>
#include <functional>

class SnakeManager;
class ChunkManager;
class World{
private:
    SnakeManager snakes;
    ChunkManager chunks;

    size_t step;

public:
    World(int size, std::vector<int> players);

    void step();

    bool setSnakeDirection(int id, Direction dir);

    std::vector<int> getSnakeList();
    std::vector<int> getAilveSnakeList();
    Direction getSnakeDirection(int id);
    SnakeStatus getSnakeStatus(int id);
    Snake::body_type getSnakeBody(int id);
    Position getSnakeHeadPos(int id);

    size_t getStepCount() const;
    size_t getMapSize() const;
    std::optional<std::reference_wrapper<std::unordered_set<Position>>> getSnakeChunk(ChunkPos);
    std::optional<std::reference_wrapper<std::unordered_set<Position>>> getFoodChunk(ChunkPos);

    const auto& getToSnake() const;
    const auto& getToFood() const;
    const auto& getToAir() const;

    const std::vector<Position> getAllHeadPos() const;
};