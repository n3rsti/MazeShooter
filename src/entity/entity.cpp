#include "src/entity/entity.h"
#include "glm/ext/vector_float3.hpp"
#include "src/maze/maze.h"
#include "src/model/model.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <queue>
#include <utility>
#include <vector>

float sx = 1.0f, sy = 1.0f, sz = 1.0f; // half-width, half-height, half-depth
bool Entity::isUnderCursor(glm::vec3 cameraPos, glm::vec3 rayDir) {
    glm::vec3 min = glm::vec3(x - sx, y, z - sz);
    glm::vec3 max = glm::vec3(x + sx, y + sy * 2.0f, z + sz);

    float tmin = (min.x - cameraPos.x) / rayDir.x;
    float tmax = (max.x - cameraPos.x) / rayDir.x;
    if (tmin > tmax)
        std::swap(tmin, tmax);

    float tymin = (min.y - cameraPos.y) / rayDir.y;
    float tymax = (max.y - cameraPos.y) / rayDir.y;
    if (tymin > tymax)
        std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (min.z - cameraPos.z) / rayDir.z;
    float tzmax = (max.z - cameraPos.z) / rayDir.z;
    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    return true;
}

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
    std::pair<float, float> player_world, float deltaTime) {
    // Convert world position to grid for pathfinding
    std::pair<int, int> entity_pos = {Maze::world_to_grid(x),
                                      Maze::world_to_grid(z)};
    std::pair<int, int> player_pos_grid = {
        Maze::world_to_grid(player_world.first),
        Maze::world_to_grid(player_world.second)};
    std::pair<int, int> next_tile =
        bfs_next_move(&maze, entity_pos, player_pos_grid);

    float target_x, target_z;

    if (entity_pos == player_pos_grid) {
        target_x = player_world.first;
        target_z = player_world.second;
    } else {
        target_x = Maze::grid_to_world(next_tile.first);
        target_z = Maze::grid_to_world(next_tile.second);
    }

    float dir_x = target_x - x;
    float dir_z = target_z - z;
    float length = std::sqrt(dir_x * dir_x + dir_z * dir_z);

    if (length > 0.01f) {
        dir_x /= length;
        dir_z /= length;

        rotation_y = std::atan2(dir_x, dir_z);

        x += dir_x * speed * deltaTime;
        z += dir_z * speed * deltaTime;

        // Snap to target if close enough
        if (std::fabs(target_x - x) < 0.1f && std::fabs(target_z - z) < 0.1f) {
            x = target_x;
            z = target_z;
        }

        // print coordinates
        std::cout << "Entity position: (" << x << ", " << y << ", " << z
                  << std::endl;
    }
}
