#include "src/entity/entity.h"
#include "src/maze/maze.h"
#include "src/model/model.h"
#include <cmath>
#include <iostream>
#include <queue>
#include <utility>
#include <vector>

std::pair<int, int> bfs_next_move(const std::vector<std::vector<int>> *maze,
                                  std::pair<int, int> entity,
                                  std::pair<int, int> player) {
    int rows = maze->size();
    int cols = maze->empty() ? 0 : (*maze)[0].size();
    std::vector<std::vector<bool>> visited(rows,
                                           std::vector<bool>(cols, false));
    std::vector<std::vector<std::pair<int, int>>> parent(
        rows, std::vector<std::pair<int, int>>(cols, {-1, -1}));

    std::queue<std::pair<int, int>> q;
    q.push(entity);
    visited[entity.second][entity.first] = true; // (y, x) order

    // 4 directions: up, down, left, right
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    bool found = false;

    while (!q.empty()) {
        auto [cx, cy] = q.front();
        q.pop();

        if (cx == player.first && cy == player.second) {
            found = true;
            break;
        }

        for (int d = 0; d < 4; ++d) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];
            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows &&
                !visited[ny][nx] && (*maze)[ny][nx] == 0) {
                visited[ny][nx] = true;
                parent[ny][nx] = {cx, cy};
                q.push({nx, ny});
            }
        }
    }

    // If no path found, stay in place
    if (!found)
        return entity;

    // Backtrack from player to entity to find the first step
    std::pair<int, int> step = player;
    std::pair<int, int> prev = player;
    while (step != entity) {
        prev = step;
        step = parent[step.second][step.first];
    }
    return prev;
}

Entity::Entity(float x, float y, float z, float speed, Model *model)
    : x(x), y(y), z(z), speed(speed), model(model) {}

void Entity::move_entity_towards_player(
    Entity &entity, const std::vector<std::vector<int>> &maze,
    std::pair<int, int> player, float deltaTime) {

    // Convert world position to grid
    std::pair<int, int> entity_pos = {Maze::world_to_grid(entity.x),
                                      Maze::world_to_grid(entity.z)};
    std::pair<int, int> next_tile = bfs_next_move(&maze, entity_pos, player);

    // If already at next tile, do nothing
    if (entity_pos == next_tile)
        return;

    // Convert next_tile (maze grid) to world coordinates
    float target_x = Maze::grid_to_world(next_tile.first);
    float target_z = Maze::grid_to_world(next_tile.second);

    float dir_x = target_x - entity.x;
    float dir_z = target_z - entity.z;
    float length = std::sqrt(dir_x * dir_x + dir_z * dir_z);

    if (length > 0.01f) {
        dir_x /= length;
        dir_z /= length;

        entity.rotation_y = std::atan2(dir_x, dir_z);

        entity.x += dir_x * entity.speed * deltaTime;
        entity.z += dir_z * entity.speed * deltaTime;

        // Snap to tile if close enough
        if (std::fabs(target_x - entity.x) < 0.1f &&
            std::fabs(target_z - entity.z) < 0.1f) {
            entity.x = target_x;
            entity.z = target_z;
        }
    }
}
