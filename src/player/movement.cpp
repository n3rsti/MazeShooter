#include <cmath>

#include "movement.h"
#include "src/maze/maze.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

Movement::Movement(float speed, Maze *maze)
    : speed(speed), x_move(0), z_move(0), maze(maze) {}

// Update movement based on key states
void Movement::updateMovement(float x_rotation,
                              std::unordered_map<int, bool> &keyStates,
                              float xpos, float zpos) {

  float cos_camera_x = cos(x_rotation / 100);
  float sin_camera_x = sin(x_rotation / 100);

  x_move = 0;
  z_move = 0;

  float tmp_x_move = 0;
  float tmp_z_move = 0;

  if (keyStates[GLFW_KEY_W]) {
    tmp_z_move += speed * cos_camera_x;
    tmp_x_move -= speed * sin_camera_x;
  }
  if (keyStates[GLFW_KEY_S]) {
    tmp_z_move -= speed * cos_camera_x;
    tmp_x_move += speed * sin_camera_x;
  }
  if (keyStates[GLFW_KEY_A]) {
    tmp_z_move += speed * sin_camera_x;
    tmp_x_move += speed * cos_camera_x;
  }
  if (keyStates[GLFW_KEY_D]) {
    tmp_z_move -= speed * sin_camera_x;
    tmp_x_move -= speed * cos_camera_x;
  }
  if (isValidMove(xpos + tmp_x_move * glfwGetTime(), zpos, tmp_x_move,
                  tmp_z_move)) {
    x_move = tmp_x_move;
  }

  if (isValidMove(xpos, zpos + tmp_z_move * glfwGetTime(), tmp_x_move,
                  tmp_z_move)) {
    z_move = tmp_z_move;
  }
}

bool Movement::isValidMove(float x, float z, float x_direction,
                           float z_direction) {

  // printf("Angle X: %f, Angle Z: %f\n", x, z);
  float blockSize = 2.0f;
  // float offsetX = maze->width- 1;
  //
  // float offsetZ = maze->height - 1;
  //
  //
  // float posX = x * blockSize - offsetX;
  // float posZ = y * blockSize - offsetZ;
  //
  float radius = 0.1;
  int my_x = (x + 1) / 2 + 35 + radius;
  int my_z = (z + 1) / 2 + 35 + radius;

  int my_x2 = (x + 1) / 2 + 35 - radius;
  int my_z2 = (z + 1) / 2 + 35 - radius;
  // print coordinates and maze value for this coordinate
  // std::cout << "Cell: " << maze->get_cell(my_z, my_x);
  return maze->get_cell(my_z, my_x) + maze->get_cell(my_z2, my_x2) +
             maze->get_cell(my_z, my_x2) + maze->get_cell(my_z2, my_x) ==
         0;
}
