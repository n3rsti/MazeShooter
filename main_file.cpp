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
#include "src/maze/maze.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <vector>
#define GLM_FORCE_RADIANS

#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "src/player/camera.h"
#include "src/player/movement.h"
#include "static/models/floor.h"
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

#include "static/models/walls.h"
// Constants
const float INITIAL_SPEED = 3.0f;
const float CAMERA_ROTATION_LIMIT = glm::pi<float>() / 2;
const float FOV = 50.0f * glm::pi<float>() / 180.0f;
const float NEAR_PLANE = 0.01f;
const float FAR_PLANE = 100.0f;

const int MAZE_WIDTH = 71;  // Width of the maze
const int MAZE_HEIGHT = 71; // Height of the maze

// Global Variables
float aspectRatio = 1;

float sensitivity = 0.5f;

ShaderProgram *sp,*csp; // Pointer to the shader program
Camera *camera;
Movement *movement;
Maze *maze;

GLuint tex0;
GLuint tex1;
GLuint grassTex;
GLuint left;
GLuint right;

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

GLuint readTexture(const char *filename) {
  GLuint tex;
  glActiveTexture(GL_TEXTURE0);

  // Read into computers memory
  std::vector<unsigned char> image; // Allocate memory
  unsigned width, height;           // Variables for image size
  // Read the image
  unsigned error = lodepng::decode(image, width, height, filename);

  // Import to graphics card memory
  glGenTextures(1, &tex);            // Initialize one handle
  glBindTexture(GL_TEXTURE_2D, tex); // Activate handle
  // Copy image to graphics cards memory reprezented by the active handle
  glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               (unsigned char *)image.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return tex;
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
  csp = new ShaderProgram("v_cross.glsl",NULL,"f_cross.glsl");
  camera = new Camera(CAMERA_ROTATION_LIMIT, FOV, NEAR_PLANE, FAR_PLANE,
                      sensitivity, aspectRatio);
  maze = new Maze(MAZE_WIDTH, MAZE_HEIGHT);
  maze->generate_maze(35, 35); // Generate the maze starting from (1, 1)

  // maze->print_maze(); // Print the generated maze to the console
  movement = new Movement(INITIAL_SPEED, maze);

  tex0 = readTexture("static/img/metal.png");
  tex1 = readTexture("static/img/sky.png");
  grassTex = readTexture("static/img/grass.png");
  left=readTexture("static/img/bron.png");
  right=readTexture("static/img/lampa.png");
}

// Free OpenGL resources
void freeOpenGLProgram(GLFWwindow *window) { delete sp; }

void drawFloor(const glm::mat4 &M) {

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
  glEnableVertexAttribArray(sp->a("vertex"));
  glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, floorVertices);

  glEnableVertexAttribArray(sp->a("texCoord0"));
  glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0,
                        floorTexCoords);

  glEnableVertexAttribArray(sp->a("normal"));
  glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, floorNormals);

  glActiveTexture(GL_TEXTURE0); // Assign texture tex0 to the 0-th texturing
                                // unit
  glBindTexture(GL_TEXTURE_2D, grassTex);
  glUniform1i(sp->u("textureMap0"),
              0); // Associate sampler textureMap0 with the 0-th texturing unit

  glDrawArrays(GL_TRIANGLES, 0, floorVertexCount); // Draw the object

  glDisableVertexAttribArray(
      sp->a("vertex")); // Disable sending data to the attribute vertex
  glDisableVertexAttribArray(
      sp->a("texCoord0")); // Disable sending data to the attribute texCoord0
  glDisableVertexAttribArray(
      sp->a("normal")); // Disable sending data to the attribute normal
}

void printMatrix(const glm::mat4 &mat) {
  for (int row = 0; row < 4; ++row) {
    std::cout << "[ ";
    for (int col = 0; col < 4; ++col) {
      std::cout << mat[col][row] << " "; // note: column-major order!
    }
    std::cout << "]\n";
  }
  std::cout << std::endl;
}
// Helper function to draw a cube
void drawCube(const glm::mat4 &M, float x, float z) {
  glm::mat4 M_translate = glm::translate(M, glm::vec3(x, 0.0f, z));

  glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_translate));
  glEnableVertexAttribArray(sp->a("vertex"));
  glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, myCubeVertices);

  glEnableVertexAttribArray(sp->a("texCoord0"));
  glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0,
                        myCubeTexCoords);

  glEnableVertexAttribArray(sp->a("normal"));
  glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, myCubeNormals);

  glActiveTexture(GL_TEXTURE0); // Assign texture tex0 to the 0-th texturing
                                // unit
  glBindTexture(GL_TEXTURE_2D, tex0);
  glUniform1i(sp->u("textureMap0"),
              0); // Associate sampler textureMap0 with the 0-th texturing unit

  glActiveTexture(GL_TEXTURE1); // Assign texture tex1 to the 1-st texturing
                                // unit
  glBindTexture(GL_TEXTURE_2D, tex1);
  glUniform1i(sp->u("textureMap1"),
              1); // Associate sampler textureMap1 with the 1-st texturing unit

  glDrawArrays(GL_TRIANGLES, 0, myCubeVertexCount); // Draw the object

  glDisableVertexAttribArray(
      sp->a("vertex")); // Disable sending data to the attribute vertex
  glDisableVertexAttribArray(
      sp->a("texCoord0")); // Disable sending data to the attribute texCoord0
  glDisableVertexAttribArray(
      sp->a("normal")); // Disable sending data to the attribute normal
}



void drawMaze(const glm::mat4 &M) {

  float blockSize = 2.0f;
  float offsetX = maze->width - 1;
  float offsetZ = maze->height - 1;

  // Draw each wall in the maze
  for (int y = 0; y < maze->height; ++y) {
    for (int x = 0; x < maze->width; ++x) {
      if (maze->maze[y][x] == 1) {
        float posX = x * blockSize - offsetX;
        float posZ = y * blockSize - offsetZ;
        drawCube(M, posX, posZ);
      }
    }
  }

  // std::cout << "Number of walls drawn: " << c << std::endl;
}
void drawCrosshair(const glm::mat4 &M) {
    glDisable(GL_DEPTH_TEST);

    glm::mat4 V = glm::mat4(1.0f);
    glm::mat4 P = glm::mat4(1.0f);

    csp->use();
    glUniformMatrix4fv(csp->u("M"), 1, GL_FALSE, glm::value_ptr(M));
    glUniformMatrix4fv(csp->u("V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(csp->u("P"), 1, GL_FALSE, glm::value_ptr(P));
    glUniform1i(csp->u("useTexture"), 0); // użyj koloru, nie tekstury

    float crosshairVertices[] = {
        -0.03f, 0.0f, 0.0f, 1.0f,
         0.03f, 0.0f, 0.0f, 1.0f,
         0.0f, -0.03f, 0.0f, 1.0f,
         0.0f,  0.03f, 0.0f, 1.0f
    };
    float crosshairColors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f
    };

    GLuint VBO[2], VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(csp->a("vertex"), 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(csp->a("vertex"));

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairColors), crosshairColors, GL_STATIC_DRAW);
    glVertexAttribPointer(csp->a("color"), 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(csp->a("color"));

    glDrawArrays(GL_LINES, 0, 4);

    glDisableVertexAttribArray(csp->a("vertex"));
    glDisableVertexAttribArray(csp->a("color"));
    glBindVertexArray(0);
    glDeleteBuffers(2, VBO);
    glDeleteVertexArrays(1, &VAO);

    glEnable(GL_DEPTH_TEST);
}
void drawHud(GLuint leftHandTex, GLuint rightHandTex) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    csp->use();
    std::cout << "Left hand tex ID: " << leftHandTex << std::endl;
    std::cout << "Right hand tex ID: " << rightHandTex << std::endl;

    glm::mat4 V = glm::mat4(1.0f);
    glm::mat4 P = glm::mat4(1.0f);

    glUniformMatrix4fv(csp->u("V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(csp->u("P"), 1, GL_FALSE, glm::value_ptr(P));
    glUniform1i(csp->u("useTexture"), 1); // użyj tekstury

    float quadVerts[] = {
        // x, y, z, w
        -1.0f, -0.5f, 0.0f, 1.0f,
        -0.4f, -0.5f, 0.0f, 1.0f,
        -0.4f, 0.4f, 0.0f, 1.0f,
        -1.0f, 0.4f, 0.0f, 1.0f
    };

    float quadUV[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
    };

    unsigned int quadIndices[] = {
        2, 1, 0,
        0, 3, 2
    };

    GLuint VAO, VBO[2], EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(csp->a("vertex"), 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(csp->a("vertex"));

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadUV), quadUV, GL_STATIC_DRAW);
    glVertexAttribPointer(csp->a("texCoord0"), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(csp->a("texCoord0"));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // lewa ręka
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, leftHandTex);
    glUniform1i(csp->u("textureMap0"), 0);
    glm::mat4 M = glm::mat4(1.0f);
    glUniformMatrix4fv(csp->u("M"), 1, GL_FALSE, glm::value_ptr(M));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // prawa ręka – przesunięcie
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rightHandTex);
    glUniform1i(csp->u("textureMap0"), 0);
    M = glm::translate(glm::mat4(1.0f), glm::vec3(1.4f, 0.0f, 0.0f));
    glUniformMatrix4fv(csp->u("M"), 1, GL_FALSE, glm::value_ptr(M));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    glDisableVertexAttribArray(csp->a("vertex"));
    glDisableVertexAttribArray(csp->a("texCoord0"));
    glBindVertexArray(0);
    glDeleteBuffers(2, VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    glEnable(GL_DEPTH_TEST);
}
// Draw scene
void drawScene(GLFWwindow *window, float x_pos, float z_pos) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  sp->use();
  camera->updateCamera(x_pos, z_pos, sp);

  glm::mat4 M = glm::mat4(1.0f);
  drawFloor(M);
  drawMaze(M);
  drawCrosshair(M);
  drawHud(left, right);
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
    movement->updateMovement(camera->x_rotation, keyStates, angle_x, angle_z);

    angle_x += movement->x_move * glfwGetTime();
    angle_z += movement->z_move * glfwGetTime();

    // printf("Angle X: %f, Angle Z: %f\n", angle_x, angle_z);
    glfwSetTime(0);

    drawScene(window, angle_x, angle_z);
    glfwPollEvents();
  }

  freeOpenGLProgram(window);
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
