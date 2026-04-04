#include <algorithm>
#include "world.h"

World::World(int sz_, std::vector<int> playerList):snakeList{}, snakePos{}, world_size(sz_){
        std::random_device rd;
        random_engine.seed(rd());

        for (auto player : playerList){
            playerJoin(player);
        }
}








ChunkPos World::unloadChunk(ChunkPos pos){
    if(!chunkMap.at(pos).snake.empty()){
        throw error::InGameException("Player exist in deleting Chunk");
    }
    for(auto f : chunkMap.at(pos).food){
        //...
    }
}

int World::playerJoin(int id_){
    if(idList.count(id_)){//禁止相同id玩家
        throw error::PlayerJoinException("id already exist");
    }

    ChunkPos spawn_chunk;
    bool found_safe = false;
    for (int i =0; i!=MAX_JOIN_SAFE_CHUNK_ATTEMP_TIMES; ++i){
        int chunk_pos_x = random_engine()%(world_size / CHUNK_SIZE);
        int chunk_pos_y = random_engine()%(world_size / CHUNK_SIZE);

        for(int dx= -SAFE_CHUNK_RADIUS; found_safe && dx!= 1+SAFE_CHUNK_RADIUS; ++dx){
            for(int dy= -SAFE_CHUNK_RADIUS; found_safe && dy!= 1+SAFE_CHUNK_RADIUS; ++dy){
                if(chunkMap.count(ChunkPos(chunk_pos_x, chunk_pos_y)) && 
                    !chunkMap.at(ChunkPos(chunk_pos_x, chunk_pos_y)).snake.empty()  ){
                    found_safe = true;
                }
            }
        }
        if(found_safe)spawn_chunk = {chunk_pos_x, chunk_pos_y};
    }

    if(!found_safe){
        throw error::PlayerJoinException("no safe spawn point found");
    }

    int spawn_pos_x = spawn_chunk.x * CHUNK_SIZE + random_engine()% (CHUNK_SIZE-INITIAL_SNAKE_LENGTH);
    int spawn_pos_y = spawn_chunk.y * CHUNK_SIZE + random_engine()% CHUNK_SIZE;

    std::deque<Position> initial_snake;
    for (int i = 0; i!= INITIAL_SNAKE_LENGTH; ++i){
        initial_snake.push_back({spawn_pos_x+i, spawn_pos_y});
        snakePos.emplace((spawn_pos_x+i, spawn_pos_y));
        loadChunk(spawn_chunk);
        chunkMap.at(spawn_chunk).snake.emplace(getPosInChunk(Position(spawn_pos_x+i,spawn_pos_y+i)));
    }
    snakeList.push_back(Snake(initial_snake, id_, Direction::Null));


    //undone ...
}

void World::addFood(){
    Position pos;
    do{
        int x = random_engine()%world_size,
            y =random_engine()%world_size;
        pos = {x, y};
    }while(snakePos.count(pos)||foodPos.count(pos));
    foodPos.emplace(pos);
    chunkMap.at(getChunkPos(pos)).food.emplace(getPosInChunk(pos));
}