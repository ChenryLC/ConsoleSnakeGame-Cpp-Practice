#pragma once
#include <../snake.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <stdexcept>
#include <optional>
#include <functional>

class SnakeManager{
    private:
        std::vector<Snake> snakes;
        std::unordered_map<int, size_t> index;//id to snakes index
        
        std::unordered_multiset<Position> head_pos_set;
        std::unordered_multiset<Position> tail_pos_set;
        std::unordered_map<int, Direction> dirs;
        std::unordered_map<Position, int> head_pos_to_id;

        size_t getIndex(int id) const;
        void refreshPosSet();
        int snake_max_length;
    public:
        int getSankeCount() const;
        std::vector<int> getAliveIds() const;
        std::vector<int> getAllId() const;

        std::optional<std::reference_wrapper<const Snake>> getSnake(int id) const;
        std::optional<Position> getHead(int id) const;
        std::optional<Position> getTail(int id) const;
        std::optional<Direction> getDirection(int id) const;

        const std::unordered_multiset<Position>& getHeadPoses() const;
        const std::unordered_multiset<Position>& getTailPoses() const;

        bool removeSnake(int id);
        bool addSnake(int id, Snake::body_type& body, Direction dir);
        bool resetSnake(int id, Snake::body_type& body, Direction dir);
        
        void clearAllSnakes();
        void clearDeadSnakes();

        void moveHeads();
        void moveTails(const std::unordered_map<int, bool>& isEat);

        bool setDead(int id);
        bool setDirection(int id, Direction dir);
        void setSnakeMaxLength(int length);

        std::optional<bool> isSelfColliding(int id) const;
        bool isAlive(int id) const;

        bool hasId(int id) const;
};
