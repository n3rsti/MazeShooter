/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "glm/common.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/ext/vector_float3.hpp"
#include <cmath>
#define GLM_FORCE_RADIANS

#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "src/player/camera.h"
#include "src/player/movement.h"
#include "static/models/myCube.h"
#include "static/models/myTeapot.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>

// Constants
const float INITIAL_SPEED = 2.0f;
const float CAMERA_ROTATION_LIMIT = glm::pi<float>() / 2;
const float FOV = 50.0f * glm::pi<float>() / 180.0f;
const float NEAR_PLANE = 1.0f;
const float FAR_PLANE = 50.0f;

// Global Variables
float aspectRatio = 1;

float sensitivity = 0.5f;

ShaderProgram *sp; // Pointer to the shader program
Camera *camera;
Movement *movement;

// Key state tracking
std::unordered_map<int, bool> keyStates;

// Error callback
void error_callback(int error, const char *description) {
  fputs(description, stderr);
}

// Key callback
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (action == GLFW_PRESS) {
    keyStates[key] = true;
  } else if (action == GLFW_RELEASE) {
    keyStates[key] = false;
  }
}

// Cursor position callback
void cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  camera->cursorPosCallback(window, xpos, ypos);
}

// Window resize callback
void windowResizeCallback(GLFWwindow *window, int width, int height) {
  if (height == 0)
    return;
  aspectRatio = static_cast<float>(width) / height;
  camera->aspect_ratio = aspectRatio;
  glViewport(0, 0, width, height);
}

// Initialize OpenGL
void initOpenGLProgram(GLFWwindow *window) {
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);
  glfwSetWindowSizeCallback(window, windowResizeCallback);
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (glfwRawMouseMotionSupported())
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

  sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
  camera = new Camera(CAMERA_ROTATION_LIMIT, FOV, NEAR_PLANE, FAR_PLANE,
                      sensitivity, aspectRatio);
  movement = new Movement(INITIAL_SPEED);
}

// Free OpenGL resources
void freeOpenGLProgram(GLFWwindow *window) { delete sp; }

// Helper function to draw a cube
void drawCube(const glm::mat4 &M) {
  glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
  glEnableVertexAttribArray(sp->a("vertex"));
  glEnableVertexAttribArray(sp->a("color"));
  glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, myCubeColors);
  glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, myCubeVertices);
  glDrawArrays(GL_TRIANGLES, 0, myCubeVertexCount);
  glDisableVertexAttribArray(sp->a("color"));
  glDisableVertexAttribArray(sp->a("vertex"));
}

void drawFloor(const glm::mat4 &M) {
  glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
  glEnableVertexAttribArray(sp->a("vertex"));
  glEnableVertexAttribArray(sp->a("color"));
  glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, myTeapotColors);
  glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0,
                        myTeapotVertices);
  glDrawArrays(GL_TRIANGLES, 0, myTeapotVertexCount);
  glDisableVertexAttribArray(sp->a("color"));
  glDisableVertexAttribArray(sp->a("vertex"));
}

void drawSurroundings() {
  glm::mat4 M = glm::mat4(1.0f);
  drawCube(M);

  glm::mat4 Mp1 = glm::translate(M, glm::vec3(-5.0f, 0.0f, -5.0f));
  drawCube(Mp1);

  glm::mat4 Mp2 = glm::translate(Mp1, glm::vec3(10.0f, 0.0f, 0.0f));
  drawCube(Mp2);
}

// Draw scene
void drawScene(GLFWwindow *window, float x_pos, float z_pos) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera->updateCamera(x_pos, z_pos, sp);
  drawSurroundings();

  glfwSwapBuffers(window);
}

// Main function
int main(void) {
  GLFWwindow *window;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    fprintf(stderr, "Can't initialize GLFW.\n");
    exit(EXIT_FAILURE);
  }

  window = glfwCreateWindow(1000, 700, "OpenGL", NULL, NULL);

  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  GLenum err;
  if ((err = glewInit()) != GLEW_OK) {
    fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
  }

  initOpenGLProgram(window);

  float angle_x = 0;
  float angle_z = 0;

  glfwSetTime(0);
  while (!glfwWindowShouldClose(window)) {
    movement->updateMovement(camera->x_rotation, keyStates);

    angle_x += movement->x_move * glfwGetTime();
    angle_z += movement->z_move * glfwGetTime();
    glfwSetTime(0);

    drawScene(window, angle_x, angle_z);
    glfwPollEvents();
  }

  freeOpenGLProgram(window);
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
