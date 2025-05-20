#include <cmath>

#include "movement.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <unordered_map>

Movement::Movement(float speed) : speed(speed), x_move(0), z_move(0) {}

// Update movement based on key states
void Movement::updateMovement(float x_rotation,
                              std::unordered_map<int, bool> &keyStates) {
  float cos_camera_x = cos(x_rotation / 100);
  float sin_camera_x = sin(x_rotation / 100);

  x_move = 0;
  z_move = 0;

  if (keyStates[GLFW_KEY_W]) {
    z_move += speed * cos_camera_x;
    x_move -= speed * sin_camera_x;
  }
  if (keyStates[GLFW_KEY_S]) {
    z_move -= speed * cos_camera_x;
    x_move += speed * sin_camera_x;
  }
  if (keyStates[GLFW_KEY_A]) {
    z_move += speed * sin_camera_x;
    x_move += speed * cos_camera_x;
  }
  if (keyStates[GLFW_KEY_D]) {
    z_move -= speed * sin_camera_x;
    x_move -= speed * cos_camera_x;
  }
}
