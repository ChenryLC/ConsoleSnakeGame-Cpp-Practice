#include "game/world.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// ============================================================================
//  Console Snake - 最小可运行演示
//
//  只用 world.h 里已经声明的接口，不碰 world.h 以外的任何东西。
//
//  用到的 World 成员（共 11 个，其中 8 个还没实现）：
//      已实现:  World(...) , setSnakeDirection() , getSnakeList()
//               getAliveSnakeList() , getSnakeStatus() , getSnakeBody()
//      待实现:  step() , getStepCount() , getSnakeDirection()
//               getSnakeHeadPos() , getSnakeChunk() , getFoodChunk()
//               getToSnake() , getToFood()
//
//  （没用到的：getMapSize() / getToAir() / getAllHeadPos()）
//
//  只想最省事地跑起来 => 把下面 RENDER 改成 0，就只需要再实现
//  step() / getStepCount() / getSnakeDirection() / getSnakeHeadPos() 四个。
// ============================================================================

#define RENDER 1          // 1 = 画地图(需要 getSnakeChunk/getFoodChunk/getToSnake/getToFood)
                          // 0 = 只打印文字状态

// ---------------------------------------------------------------------------
// 画一帧地图：地图是 16x16 一个 chunk，这里只画以 origin 为左下角、w x h 的一块
//
// 这里的 getSnakeChunk/getFoodChunk 是「按需加载」接口：只有先请求过的 chunk
// 才会返回内容，所以每帧都要把视野内的每个 chunk 都问一遍。查询本身就会
// 把没加载的 chunk 加载进来。
// ---------------------------------------------------------------------------
#if RENDER

const int CHUNK_SIDE = 16;   // 与 common/constant.h 的 CHUNK_SIZE 一致

void drawMap(World& world, int origin_x, int origin_y, int w, int h){
    // 1) 先收集视野内所有 chunk 里的蛇和食物（世界坐标）
    std::vector<Position> snake_cells;
    std::vector<Position> food_cells;

    int cx_from = origin_x / CHUNK_SIDE;
    int cx_to   = (origin_x + w) / CHUNK_SIDE;
    int cy_from = origin_y / CHUNK_SIDE;
    int cy_to   = (origin_y + h) / CHUNK_SIDE;

    for (int cy = cy_from; cy <= cy_to; ++cy){
        for (int cx = cx_from; cx <= cx_to; ++cx){
            ChunkPos cp(cx, cy);

            auto sc = world.getSnakeChunk(cp);
            if (sc){
                for (const Position& p : sc.value().get()) snake_cells.push_back(p);
            }
            auto fc = world.getFoodChunk(cp);
            if (fc){
                for (const Position& p : fc.value().get()) food_cells.push_back(p);
            }
        }
    }

    // 2) 用「变更记录」拿到这一步新加进来的东西，画成高亮
    //    注意：这些是 std::vector<Position> 的拷贝，不要拿引用
    std::vector<Position> new_snake = world.getToSnake();
    std::vector<Position> new_food  = world.getToFood();

    // 3) 从上往下逐行打印（y 越大越靠上，和 moveHead 里 Up = y+1 对应）
    auto contains = [](const std::vector<Position>& v, const Position& p){
        return std::find(v.begin(), v.end(), p) != v.end();
    };

    std::string line;
    std::cout << '+';
    for (int i = 0; i < w; ++i) std::cout << '-';
    std::cout << "+\n";

    for (int y = origin_y + h - 1; y >= origin_y; --y){
        line.clear();
        for (int x = origin_x; x < origin_x + w; ++x){
            Position p(x, y);

            auto it_snake = std::find(snake_cells.begin(), snake_cells.end(), p);
            if (it_snake != snake_cells.end()){
                line += contains(new_snake, p) ? 'O' : 'o';
                continue;
            }
            auto it_food = std::find(food_cells.begin(), food_cells.end(), p);
            if (it_food != food_cells.end()){
                line += contains(new_food, p) ? '*' : '.';
                continue;
            }
            line += ' ';
        }
        std::cout << '|' << line << "|\n";
    }

    std::cout << '+';
    for (int i = 0; i < w; ++i) std::cout << '-';
    std::cout << "+\n";
    std::cout << "O/o = 蛇(新/旧)   */. = 食物(新/旧)\n\n";
}

#endif // RENDER

// ---------------------------------------------------------------------------
// 选下一步方向。
//
// 这个函数存在的唯一原因：world.h 里既没有「撞墙/撞自己」的查询接口，
// 也没有「蛇头在地图哪个范围合法」的接口，所以只能自己躲着边界走 ——
// 否则蛇会直接撞死。判断依据全部来自 world.h 暴露的接口。
//
// 如果 getFoodChunk 没被实现（isEmpty=true），就退化成贴边转圈。
// ---------------------------------------------------------------------------
Direction chooseDirection(World& world, int id, int map_size, bool food_known){
    std::vector<int> alive = world.getAliveSnakeList();
    if (alive.empty()) return Direction::Null;

    // setSnakeDirection 禁止 180 度掉头，所以这里要先知道当前方向，避免白叫
    Direction cur = world.getSnakeDirection(id).value_or(Direction::Null);
    Position head = world.getSnakeHeadPos(id);

    const int margin = 2;   // 离边界留几格就提前转身
    bool too_left   = head.x <= margin;
    bool too_right  = head.x >= map_size - 1 - margin;
    bool too_bottom = head.y <= margin;
    bool too_top    = head.y >= map_size - 1 - margin;

    // --- 有食物就朝食物贪心走 ---
    if (food_known){
        const std::vector<Position>& food = world.getToFood();
        if (!food.empty()){
            const Position& target = food.front();
            bool want_left  = target.x < head.x;
            bool want_right = target.x > head.x;
            bool want_up    = target.y > head.y;
            bool want_down  = target.y < head.y;

            // 用边界条件把不安全的选项砍掉
            if (want_left  && !too_left)                          return Direction::Left;
            if (want_right && !too_right)                         return Direction::Right;
            if (want_up    && !too_top)                           return Direction::Up;
            if (want_down  && !too_bottom)                        return Direction::Down;
        }
    }

    // --- 没食物 / 食物不可达：贴着边界顺时针转圈 ---
    if (!too_right  && cur != Direction::Left)  return Direction::Right;
    if (!too_top    && cur != Direction::Down)  return Direction::Up;
    if (!too_left   && cur != Direction::Right) return Direction::Left;
    if (!too_bottom && cur != Direction::Up)    return Direction::Down;

    // --- 最后兜底 ---
    // 绝对不能返回 Null：moveHead(Null) 会让蛇原地不动，而且外部也没法
    // 再把它救回来（连 setSnakeDirection(id, Up) 也会因为“禁止掉头”被忽略）。
    // 按优先级挑第一个不是掉头的方向。
    if (cur != Direction::Left)  return Direction::Right;
    if (cur != Direction::Down)  return Direction::Up;
    if (cur != Direction::Right) return Direction::Left;
    if (cur != Direction::Up)    return Direction::Down;
    return Direction::Up;
}

int main(){
    // --- 配置 ---
    // 注意：map_size 请取 16 的整数倍。ChunkManager 构造函数里是
    //       world_size = map_size & ~15（向 16 取整），
    // 非 16 倍数的值会被悄悄截断，而这里的边界判断用的是原值。
    const int map_size = 32;                 // 地图边长(格)
    std::vector<int> players{0, 1};          // 玩家 id，构造函数会直接把他们放进世界
    const int tick_ms  = 100;                // 每步间隔(毫秒)
    const size_t max_steps = 1000;           // 演示多久后自动停

    std::cout << "=== Console Snake demo ===\n";
    std::cout << "map_size = " << map_size
              << ", players =";
    for (int p : players) std::cout << ' ' << p;
    std::cout << "\n\n";

    World world(map_size, players);

    // 用 getSnakeList() 确认构造函数真的把蛇放进去了。
    // (它不是 const 成员，所以必须用一个非 const 的 World 对象来调)
    std::vector<int> all = world.getSnakeList();
    std::cout << "snakes in world: " << all.size() << "\n";
    for (int id : all){
        auto st = world.getSnakeStatus(id);
        std::cout << "  id=" << id
                  << " status=" << (st ? static_cast<int>(st.value()) : -1)
                  << "\n";
    }
    std::cout << "\n";

    // 第 0 步先把视野铺开（也让 getToSnake/getToFood 有内容可读），
    // 之后靠这一句判断食物接口到底实现了没有。
    world.step();
    const bool food_known = !world.getToFood().empty();

    size_t i = 0;
    for (; i < max_steps; ++i){
        std::vector<int> alive = world.getAliveSnakeList();
        if (alive.empty()){
            std::cout << "all snakes are dead, stop at step " << i << "\n";
            break;
        }

#if RENDER
        std::cout << "step " << world.getStepCount()
                  << "   alive=" << alive.size() << "\n";
        drawMap(world, 0, 0, map_size, map_size);
#else
        std::cout << "step " << world.getStepCount()
                  << "   alive=" << alive.size();
        for (int id : alive){
            Position h = world.getSnakeHeadPos(id);
            std::cout << "   id" << id << "@(" << h.x << "," << h.y << ")";
        }
        std::cout << "\n";
#endif

        // --- 操控：给每条活着的蛇算一个方向 ---
        for (int id : alive){
            Direction dir = chooseDirection(world, id, map_size, food_known);
            if (dir != Direction::Null){
                world.setSnakeDirection(id, dir);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms));
        world.step();   // 推进一帧：移动蛇头/蛇尾、判定碰撞、生成食物、卸载 chunk
    }

    // --- 收尾统计 ---
    std::cout << "\n=== finished after " << i << " steps ===\n";
    for (int id : world.getSnakeList()){
        auto st = world.getSnakeStatus(id);
        std::cout << "  id=" << id
                  << " status=" << (st ? static_cast<int>(st.value()) : -1);

        // getSnakeBody 返回的是 optional<reference_wrapper<const deque<Position>>>
        auto body = world.getSnakeBody(id);
        if (body){
            std::cout << " length=" << body.value().get().size();
        }
        std::cout << "\n";
    }

    return 0;
}
