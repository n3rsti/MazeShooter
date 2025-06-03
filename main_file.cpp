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

#include "assimp/mesh.h"
#include "glm/common.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/ext/vector_float3.hpp"
#include "src/maze/maze.h"
#include "src/model/model.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <string>
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
#include "static/models/walls.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
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

ShaderProgram *sp; // Pointer to the shader program
Camera *camera;
Movement *movement;
Maze *maze;
Model *treeModel;

GLuint tex0;
GLuint tex1;
GLuint grassTex;

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
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, (unsigned char *)image.data());

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
    camera = new Camera(CAMERA_ROTATION_LIMIT, FOV, NEAR_PLANE, FAR_PLANE,
                        sensitivity, aspectRatio);
    maze = new Maze(MAZE_WIDTH, MAZE_HEIGHT);
    maze->generate_maze(35, 35); // Generate the maze starting from (1, 1)

    // maze->print_maze(); // Print the generated maze to the console
    movement = new Movement(INITIAL_SPEED, maze);

    tex0 = readTexture("static/img/metal.png");
    tex1 = readTexture("static/img/sky.png");
    grassTex = readTexture("static/img/grass.png");
    treeModel = new Model();
    treeModel->loadModel(std::string("static/models/Tree01_OBJ.obj"), sp);
}

// Free OpenGL resources
void freeOpenGLProgram(GLFWwindow *window) { delete sp; }

void drawFloor(const glm::mat4 &M) {

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0,
                          floorVertices);

    glEnableVertexAttribArray(sp->a("texCoord0"));
    glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0,
                          floorTexCoords);

    glEnableVertexAttribArray(sp->a("normal"));
    glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, floorNormals);

    glActiveTexture(GL_TEXTURE0); // Assign texture tex0 to the 0-th texturing
                                  // unit
    glBindTexture(GL_TEXTURE_2D, grassTex);
    glUniform1i(
        sp->u("textureMap0"),
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
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0,
                          myCubeVertices);

    glEnableVertexAttribArray(sp->a("texCoord0"));
    glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0,
                          myCubeTexCoords);

    glEnableVertexAttribArray(sp->a("normal"));
    glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0,
                          myCubeNormals);

    glActiveTexture(GL_TEXTURE0); // Assign texture tex0 to the 0-th texturing
                                  // unit
    glBindTexture(GL_TEXTURE_2D, tex0);
    glUniform1i(
        sp->u("textureMap0"),
        0); // Associate sampler textureMap0 with the 0-th texturing unit

    glActiveTexture(GL_TEXTURE1); // Assign texture tex1 to the 1-st texturing
                                  // unit
    glBindTexture(GL_TEXTURE_2D, tex1);
    glUniform1i(
        sp->u("textureMap1"),
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

// Draw scene
void drawScene(GLFWwindow *window, float x_pos, float z_pos) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sp->use();
    camera->updateCamera(x_pos, z_pos, sp);

    glm::mat4 M = glm::mat4(1.0f);
    drawFloor(M);
    drawMaze(M);

    treeModel->draw(M); // Draw the tree model

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
        movement->updateMovement(camera->x_rotation, keyStates, angle_x,
                                 angle_z);

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
