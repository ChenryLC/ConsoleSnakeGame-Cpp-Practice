#include <algorithm>
#include "world.h"

World::World(int sz_, std::vector<int> joinIdList, std::shared_ptr<IGameEventHandler> game_event_handler_)
    :map_size(sz_/CHUNK_SIZE), game_event_handler(game_event_handler_){
    std::random_device rd;
    random_engine.seed(rd());

    if(game_event_handler==nullptr)throw std::invalid_argument("game_event_handler cannot be null");

    for (auto p : joinIdList){
        playerJoin(p);
    }

    }

void World::chunkUpdate(){
    for(auto cpos : chunkUpdateList){
        auto it = chunkList.find(cpos);
        if (it == chunkList.end()) continue;
        auto& chunk = it->second;
        
        if(chunk.load_level & 1){
            for(int i=0; i != INITIAL_FOOD_COUNT_PER_CHUNK; ++i){
                chunk.food.insert(LocalPos(random_engine()%CHUNK_SIZE, random_engine()%CHUNK_SIZE));
            }
            chunk.load_level &= ~1;
        }
        if(chunk.load_level == 0){
            chunkList.erase(it);
        }
    }
    chunkUpdateList.clear();
}

int World::playerJoin(int id_){
    if(std::find(idList.begin(), idList.end(), id_) != idList.end()){
        throw error::PlayerJoinException("player id already exist");
    }

    ChunkPos spawn_chunk;
    bool found_safe_chunk=false;
    for (int i=0; i!=MAX_JOIN_SAFE_CHUNK_ATTEMP_TIMES&& !found_safe_chunk; ++i){
        int x = random_engine()%map_size;//以区块计
        int y = random_engine()%map_size;
        bool danger = false;
        for(int j=-SAFE_CHUNK_RADIUS; j!=SAFE_CHUNK_RADIUS+1 && !danger; ++j){
            for(int k=-SAFE_CHUNK_RADIUS; k!=SAFE_CHUNK_RADIUS+1 && !danger; ++k){
                if(chunkList.count({x+j,y+k})&&!chunkList.at({x+j,y+k}).snake.empty()){
                    danger = true;
                }
            }
        }
        if(!danger){
            found_safe_chunk = true;
            spawn_chunk = ChunkPos(x, y);
        }
    }
    if(!found_safe_chunk){
        throw error::PlayerJoinException("no safe spawn pos found");
    }

    for(int i=-SAFE_CHUNK_RADIUS; i!=SAFE_CHUNK_RADIUS+1; ++i){
        for(int j=-SAFE_CHUNK_RADIUS; j!=SAFE_CHUNK_RADIUS+1; ++j){
            chunkList.emplace(std::make_pair(ChunkPos(spawn_chunk.x+i, spawn_chunk.y+j), Chunk()));
            chunkList.at(ChunkPos(spawn_chunk.x+i, spawn_chunk.y+j)).load_level+=2;//将第1位的值加一
            chunkUpdateList.push_back(ChunkPos(spawn_chunk.x+i, spawn_chunk.y+j));
        }
    }
    int spawn_pos_x = random_engine()%CHUNK_SIZE;
    int spawn_pos_y = random_engine()%(CHUNK_SIZE-INITIAL_SNAKE_LENGTH);//intiial_snake_size需保证小于chunk_size(已保证)
    std::deque<Position> body;
    for(int i=0; i!=INITIAL_SNAKE_LENGTH; ++i){
        body.push_back(Position(spawn_chunk.x*CHUNK_SIZE+spawn_pos_x, spawn_chunk.y*CHUNK_SIZE+spawn_pos_y+i));
        chunkList.at(spawn_chunk).snake.insert(LocalPos(spawn_pos_x, spawn_pos_y+i));
    }
    snakeList.push_back(Snake(body, id_, Direction::Up));
    playerDirection.emplace(id_, Direction::Null);
    chunkPosOfPlayer.emplace(id_, spawn_chunk);

    idList.push_back(id_);

    return id_;
}

void World::addFood(ChunkPos c){
    LocalPos pos;
    int attempt_times=0;
    bool has_added=false;
    do {
        pos=LocalPos(random_engine()%CHUNK_SIZE, random_engine()%CHUNK_SIZE);
        if(!chunkList[c].food.count(pos) && !chunkList[c].snake.count(pos)){
            has_added=true;
        }else ++attempt_times;
        if(attempt_times>=MAX_FOOD_ADD_ATTEMP_TIMES){
            throw error::InGameException("cannot add food: chunk too full");
        }
    }while(!has_added);
    return;
}

void World::step(){
    dirtyChunkList.clear();
    if(snakeList.empty()) throw error::InGameException("no snake in game");
    //头移动
    std::unordered_multiset<Position> newHeadPos;
    for(auto &s : snakeList){
        auto pos = s.moveHead(playerDirection.at(s.getId()));
        if(pos.has_value()){
            dirtyChunkList.push_back(getChunkPos(pos.value()));
            newHeadPos.emplace(pos.value());
        }
    }
    //是否进食
    std::vector<bool> isEat;
    for(auto &s : snakeList){
        if(chunkList.at(getChunkPos(s.getHead())).food.count(toLocalPos(s.getHead()))){
            isEat.push_back(true);
            chunkList.at(getChunkPos(s.getHead())).food.erase(toLocalPos(s.getHead()));
            addFood(getChunkPos(s.getHead()));
        }else{
            isEat.push_back(false);
        }
    }

    //尾移动
    std::vector<std::optional<Position>> oldTailPos;
    for(int i=0; auto &s : snakeList){
        auto pos = s.moveTail(isEat.at(i));
        if(pos.has_value()){     
            oldTailPos.push_back(pos);
            chunkList.at(getChunkPos(pos.value())).snake.erase(toLocalPos(pos.value()));
            dirtyChunkList.push_back(getChunkPos(pos.value()));
        }else{
            oldTailPos.push_back(std::nullopt);
        }
        ++i;
    }

    //碰撞检测
    for(auto &s : snakeList){
        if (s.getHead().x<0||s.getHead().y<0||
            s.getHead().x>=map_size*CHUNK_SIZE||s.getHead().y>=map_size*CHUNK_SIZE){
            s.setStatus(SnakeStatus::Dead);
        }else if(chunkList.at(getChunkPos(s.getHead())).snake.count(toLocalPos(s.getHead()))||
            newHeadPos.count(s.getHead())>=2){
            s.setStatus(SnakeStatus::Dead);
        }else{
            chunkList.at(getChunkPos(s.getHead())).snake.emplace(toLocalPos(s.getHead()));
        }
    }

    //区块逻辑
    for(int c = 0; Snake &s : snakeList){
        if(getChunkPos(s.getHead())!=chunkPosOfPlayer[s.getId()]){
            ChunkPos newChunkPos=getChunkPos(s.getHead());
            chunkPosOfPlayer.at(s.getId())=newChunkPos;            
            switch(playerDirection.at(s.getId())){
                case Direction::Down:
                for(int i=-CHUNK_LOAD_RADIUS; i!=CHUNK_LOAD_RADIUS+1; ++i){
                    ChunkPos cpos = ChunkPos(newChunkPos.x+i, newChunkPos.y-CHUNK_LOAD_RADIUS);
                    chunkUpdateList.push_back(cpos);
                    chunkList[cpos].load_level+=2;
                }
                break;
                case Direction::Up:
                for(int i=-CHUNK_LOAD_RADIUS; i!=CHUNK_LOAD_RADIUS+1; ++i){
                    ChunkPos cpos = ChunkPos(newChunkPos.x+i, newChunkPos.y+CHUNK_LOAD_RADIUS);
                    chunkUpdateList.push_back(cpos);
                    chunkList[cpos].load_level+=2;
                }
                break;
                case Direction::Right:
                for(int i=-CHUNK_LOAD_RADIUS; i!=CHUNK_LOAD_RADIUS+1; ++i){
                    ChunkPos cpos = ChunkPos(newChunkPos.x+CHUNK_LOAD_RADIUS, newChunkPos.y+i);
                    chunkUpdateList.push_back(cpos);
                    chunkList[cpos].load_level+=2;
                }
                break;
                case Direction::Left:
                for(int i=-CHUNK_LOAD_RADIUS; i!=CHUNK_LOAD_RADIUS+1; ++i){
                    ChunkPos cpos = ChunkPos(newChunkPos.x-CHUNK_LOAD_RADIUS, newChunkPos.y+i);
                    chunkUpdateList.push_back(cpos);
                    chunkList[cpos].load_level+=2;
                }
                break;
            }
        }
        
        if(oldTailPos[c].has_value() && getChunkPos(s.getTail())!=getChunkPos(oldTailPos[c].value())){
            ChunkPos oldChunkPos = getChunkPos(oldTailPos[c].value());
            switch(playerDirection.at(s.getId())){
                case Direction::Down:
                for(int i=-CHUNK_LOAD_RADIUS; i!=CHUNK_LOAD_RADIUS+1; ++i){
                    ChunkPos cpos = ChunkPos(oldChunkPos.x+i, oldChunkPos.y+CHUNK_LOAD_RADIUS);
                    chunkUpdateList.push_back(cpos);
                    chunkList[cpos].load_level-=2;
                }
                break;
                case Direction::Up:
                for(int i=-CHUNK_LOAD_RADIUS; i!=CHUNK_LOAD_RADIUS+1; ++i){
                    ChunkPos cpos = ChunkPos(oldChunkPos.x+i, oldChunkPos.y-CHUNK_LOAD_RADIUS);
                    chunkUpdateList.push_back(cpos);
                    chunkList[cpos].load_level-=2;
                }
                break;
                case Direction::Right:
                for(int i=-CHUNK_LOAD_RADIUS; i!=CHUNK_LOAD_RADIUS+1; ++i){
                    ChunkPos cpos = ChunkPos(oldChunkPos.x-CHUNK_LOAD_RADIUS, oldChunkPos.y+i);
                    chunkUpdateList.push_back(cpos);
                    chunkList[cpos].load_level-=2;
                }
                break;
                case Direction::Left:
                for(int i=-CHUNK_LOAD_RADIUS; i!=CHUNK_LOAD_RADIUS+1; ++i){
                    ChunkPos cpos = ChunkPos(oldChunkPos.x+CHUNK_LOAD_RADIUS, oldChunkPos.y+i);
                    chunkUpdateList.push_back(cpos);
                    chunkList[cpos].load_level-=2;
                }
                break;
            }
        }
    }
    std::sort(chunkUpdateList.begin(), chunkUpdateList.end());
    auto chunkUniqueBeginPos =std::unique(chunkUpdateList.begin(), chunkUpdateList.end());
    chunkUpdateList.erase(chunkUniqueBeginPos, chunkUpdateList.end());
    chunkUpdate();
    //蛇死亡逻辑
    std::vector<int> deadId;
    for(size_t i=0; i!=snakeList.size(); ++i){
        if(snakeList[i].getStatus()==SnakeStatus::Dead){
            deadId.push_back(snakeList[i].getId());
        }
    }
    for(int i : deadId){
        game_event_handler->playerDied(i);
        playerLeave(i);
    }
    std::sort(dirtyChunkList.begin(), dirtyChunkList.end());
    dirtyChunkList.erase(std::unique(dirtyChunkList.begin(), dirtyChunkList.end()), dirtyChunkList.end());
    ++step_count;
}

bool World::playerLeave(int id){
    auto snake = std::find_if(snakeList.begin(), snakeList.end(), [id](Snake& s){return s.getId()==id;});
    if(snake==snakeList.end()) return false;
    std::vector<ChunkPos> leaveChunk;
    for(auto &p : (*snake).getBody()){
        chunkList[getChunkPos(p)].snake.erase(toLocalPos(p));
        leaveChunk.push_back(getChunkPos(p));
    }
    for(int i=getChunkPos((*snake).getHead()).x-CHUNK_LOAD_RADIUS; i!=getChunkPos((*snake).getHead()).x+CHUNK_LOAD_RADIUS; ++i){
        for(int j=getChunkPos((*snake).getHead()).y-CHUNK_LOAD_RADIUS; j!=getChunkPos((*snake).getHead()).y+CHUNK_LOAD_RADIUS; ++j){
            leaveChunk.push_back(ChunkPos(i, j));
        }
    }
    std::sort(leaveChunk.begin(), leaveChunk.end());
    leaveChunk.erase(std::unique(leaveChunk.begin(), leaveChunk.end()), leaveChunk.end());
    for(auto p : leaveChunk){
        chunkUpdateList.push_back(p);
        dirtyChunkList.push_back(p);
        if(chunkList.at(p).load_level>=2)
        chunkList.at(p).load_level-=2;
    }
    using std::swap;
    swap(*snake, snakeList.back());
    snakeList.pop_back();
    swap(*std::find(idList.begin(), idList.end(), id), idList.back());
    idList.pop_back();
    playerDirection.erase(id);
    chunkPosOfPlayer.erase(id);
    return true;
}

bool World::setPlayerDirection(int id, Direction dir){
    if(playerDirection.find(id)!=playerDirection.end())
        playerDirection[id]=dir;
    else
        return false;
    return true;
}

std::unordered_map<int, bool> World::setMultiplePlayerDirection(const std::unordered_map<int, Direction> pos){
    std::unordered_map<int, bool> b;
    for(auto p : pos){
        b.insert(std::make_pair(p.first, setPlayerDirection(p.first, p.second)));
    }
    return b;
}

const Position& World::getPlayerHeadPos(int id) const{
    return (*std::find_if(snakeList.begin(), snakeList.end(), [id](Snake &s){return s.getId()==id;})).getBody().front();
}

const Chunk& World::getChunk(const ChunkPos &p) const{
    return chunkList.at(p);
}

const std::optional<Snake> World::getSnake(int id) const{
    auto s =std::find_if(snakeList.begin(), snakeList.end(), [id](const Snake &s){return s.getId()==id;});
    if(s==snakeList.end())return std::nullopt;
    return *s;
}

std::vector<int> World::getActivePlayerIdList() const{
    std::vector<int> id_list;
    for(auto s : snakeList){
        if(s.getStatus()==SnakeStatus::Alive){
            id_list.push_back(s.getId());
        }
    }
    return id_list;
}

bool World::isPlayerAlive(int id) const{
    auto it=std::find_if(snakeList.begin(), snakeList.end(), [id](Snake &s){return s.getId()==id;});
    if(it==snakeList.end()) return false;
    auto s=(*it).getStatus();
    return (s==SnakeStatus::Alive);
}

std::vector<ChunkPos> World::getLoadedChunkPos() const{
    std::vector<ChunkPos> LoadedChunkList;
    for(auto c : chunkList){
        LoadedChunkList.push_back(c.first);
    }
    return LoadedChunkList;
}

std::vector<int> World::getAllPlayerIdList() const{
    return idList;
}