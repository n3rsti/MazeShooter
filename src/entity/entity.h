#ifndef ENTITY_H
#define ENTITY_H

#include "glm/ext/vector_float3.hpp"
#include "src/model/model.h"
#include <utility>
#include <vector>

// In Entity.h

class Entity {
  public:
    float x;
    float y;
    float z;
    float speed;
    float rotation_y;
    Model *model;

    Entity(float x, float y, float z, float speed, Model *model);
    void move_entity_towards_player(Entity &entity,
                                    const std::vector<std::vector<int>> &maze,
                                    std::pair<float, float> player_world,
                                    float deltaTime);

    bool isUnderCursor(glm::vec3 cameraPos, glm::vec3 rayDir);
};

#endif
