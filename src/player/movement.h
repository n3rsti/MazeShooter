
#include "src/maze/maze.h"
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

class Movement {
public:
  float x_move;
  float z_move;
  float speed;
  Maze *maze;

  Movement(float speed, Maze *maze);

  // Update movement based on key states
  void updateMovement(float x_rotation,
                      std::unordered_map<int, bool> &keyStates, float xpos, float zpos);
    bool isValidMove(float x, float z, float x_direction, float z_direction);
};
