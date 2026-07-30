#include "../../../common/chunk.h"
#include "../../../common/constant.h"
#include "../../../common/local_pos.h"
#include "../../../common/position.h"
#include "../../error/game_error.h"
#include <unordered_map>
#include <vector>
#include <optional>
#include <functional>
#include <random>

class ChunkManager{
        class DelayUnloadList{
            private:
                std::vector<ChunkPos> buffer;
                std::vector<ChunkPos> unload_list;
                size_t head_index;
                size_t tail_index;
                size_t count;
                size_t capacity;

            public:
                bool need_unload;
                DelayUnloadList(size_t buffer_size);
                void add(const ChunkPos&);
                void clear(size_t count);
                void clear();
                bool hasChunk(ChunkPos);
                bool empty();
                size_t size();

        };

    private:
        static inline constexpr int chunk_size = 16;
        const int load_radius;
        const int safe_radius;
        const size_t world_size;

        std::uniform_int_distribution<int>& random_engine;

        std::unordered_map<ChunkPos, std::unordered_set<LocalPos>> snake_chunks;
        std::unordered_map<ChunkPos, std::unordered_set<LocalPos>> food_chunks;
        std::unordered_map<ChunkPos, int> load_level;
        std::vector<ChunkPos> load_list;
        DelayUnloadList unload_list{128};

        void unloadChunks(ChunkPos);
    public:
        ChunkManager(int map_size, int load_radius, int spawn_safe_radius, std::uniform_int_distribution<int>& random_engine);

        bool hasChunk(ChunkPos) const;
        bool loadChunk(ChunkPos);//load chunk if possible
        bool ensureLoadChunk(ChunkPos);//load chunk when it's unloaded
        bool increaseLoadLevel(ChunkPos);//add 1 to load reference count
        bool decreaseLoadLevel(ChunkPos);//minus 1 to load reference count
        bool addFood(ChunkPos);
        bool hasFood(Position);
        bool popFood(Position);
        bool insertSnake(Position);
        bool popSnake(Position);
        bool hasSnake(Position);
        
};