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
                std::vector<ChunkPos> buffer_{128};
                std::vector<ChunkPos> unload_list_{64};
                size_t head_index_ =0;
                size_t tail_index_ =0;
                size_t count_ =0;
                static constexpr size_t capacity_ =128;

            public:
                bool need_unload = false;
                DelayUnloadList(/*size_t buffer_size*/)=default;
                void add(const ChunkPos);
                void clear(size_t count);
                void clear();
                bool empty() const;
                bool full() const;
                size_t size() const;
                size_t capacity() const;
                std::vector<ChunkPos>& getUnloadList();
        };

    private:
        static inline constexpr int chunk_size = 16;
        const size_t world_size;
        const size_t world_size_by_chunk;

        std::mt19937 random_gen;
        std::uniform_int_distribution<int> random_range;

        std::unordered_map<ChunkPos, std::unordered_set<LocalPos>> snake_chunks;
        std::unordered_map<ChunkPos, std::unordered_set<LocalPos>> food_chunks;
        std::unordered_map<ChunkPos, int> load_level;
        DelayUnloadList unload_list /*{static_cast<size_t>(128)}*/;

        void unloadChunks(ChunkPos);
    public:
        ChunkManager(int map_size, int load_radius, std::mt19937& random_gen_);

        bool hasChunk(ChunkPos) const;
        bool loadChunk(ChunkPos);//load chunk if possible
        bool increaseLoadLevel(ChunkPos);//add 1 to load reference count
        bool decreaseLoadLevel(ChunkPos);//minus 1 to load reference count
        bool addFood(ChunkPos);
        bool hasFood(Position) const;
        bool popFood(Position);
        bool insertSnake(Position);
        bool popSnake(Position);
        bool hasSnake(Position) const;
        
        void flushUnload();

        void clear();
};