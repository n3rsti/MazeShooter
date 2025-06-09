#ifndef ENTITY_H
#define ENTITY_H

#include "src/model/model.h"
#include <utility>
#include <vector>
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
                                    std::pair<int, int> player,
                                    float deltaTime);
};

#endif
