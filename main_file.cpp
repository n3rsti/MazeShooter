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

#include "glm/ext/matrix_transform.hpp"
#define GLM_FORCE_RADIANS

#include "constants.h"
#include "lodepng.h"
#include "myCube.h"
#include "myTeapot.h"
#include "shaderprogram.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>

float move_x = 0;  // angular speed in radiansmain
float speed_y = 0; // angular speed in radians
float move_z = 0;
float aspectRatio = 1;

float camera_x = 0;
float camera_y = 0;

float y_rotation = 0;
int movement_keys_clicked = 0;

float angle = 0.0f;

float previous_camera_x = 0;
float previous_camera_y = 0;

float speed = 2;
ShaderProgram *sp; // Pointer to the shader program
// Error processing callback procedure
void error_callback(int error, const char *description) {
  fputs(description, stderr);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {

  printf("Data: (%f, %f)\n", move_x, move_z);

  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_W) {
      move_z += speed * cos(camera_x / 100);
      move_x -= speed * sin(camera_x / 100);

      movement_keys_clicked++;
    }
    if (key == GLFW_KEY_S) {

      move_z -= speed * cos(camera_x / 100);
      move_x += speed * sin(camera_x / 100);

      movement_keys_clicked++;
    }
    if (key == GLFW_KEY_D) {

      move_z -= speed * sin(camera_x / 100);
      move_x -= speed * cos(camera_x / 100);

      movement_keys_clicked++;
    }
    if (key == GLFW_KEY_A) {
      move_z += speed * sin(camera_x / 100);
      move_x += speed * cos(camera_x / 100);

      movement_keys_clicked++;
    }
  }

  if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_W) {
      move_z -= speed * cos(camera_x / 100);
      move_x += speed * sin(camera_x / 100);

      movement_keys_clicked--;
    }
    if (key == GLFW_KEY_S) {

      move_z += speed * cos(camera_x / 100);
      move_x -= speed * sin(camera_x / 100);

      movement_keys_clicked--;
    }
    if (key == GLFW_KEY_D) {

      move_z += speed * sin(camera_x / 100);
      move_x += speed * cos(camera_x / 100);

      movement_keys_clicked--;
    }
    if (key == GLFW_KEY_A) {
      move_z -= speed * sin(camera_x / 100);
      move_x -= speed * cos(camera_x / 100);

      movement_keys_clicked--;
    }

    if (movement_keys_clicked == 0) {
      move_x = 0;
      move_z = 0;
    }
  }
}

void cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  // print the cursor position
  // old_xpos - xpos = camerax
  // old_xpos=xpos
  //

  camera_x -= previous_camera_x - xpos;

  y_rotation -= (previous_camera_y - ypos) / 100;
  if (y_rotation > PI / 2) {
    y_rotation = PI / 2;
  } else if (y_rotation < -PI / 2) {
    y_rotation = -PI / 2;
  }

  previous_camera_x = xpos;
  previous_camera_y = ypos;

  printf("Cursor position: (%f, %f, %f, %f)\n", xpos, ypos, camera_x,
         y_rotation);
}

void windowResizeCallback(GLFWwindow *window, int width, int height) {
  if (height == 0)
    return;
  aspectRatio = (float)width / (float)height;
  glViewport(0, 0, width, height);
}

// Initialization code procedure
void initOpenGLProgram(GLFWwindow *window) {
  //************Place any code here that needs to be executed once, at the
  // program start************
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
}

// Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow *window) {
  //************Place any code here that needs to be executed once, after the
  // main loop ends************
  delete sp;
}

// Drawing procedure
void drawScene(GLFWwindow *window, float angle_x, float angle_z,
               float camera_x) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::vec3 eye = glm::vec3(angle_x, 0.0f, angle_z - 5);
  glm::vec3 center = glm::vec3(angle_x, 0.0f, 1);
  glm::vec3 relative = center - eye;

  // Rotate the camera around the y-axis and limit the rotation to 360 degrees
  //
  //

  center.y = relative.y * cos(y_rotation) - relative.z * sin(y_rotation);

  // fprintf(stderr, "Camera Y: %f\n", y_rotation);

  center.x =
      relative.x * cos(camera_x / 100) - relative.z * sin(camera_x / 100);
  center.z =
      relative.x * sin(camera_x / 100) + relative.z * cos(camera_x / 100);

  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

  glm::mat4 V = glm::lookAt(eye, center + eye, up);

  glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, 1.0f, 1.0f,
                                 50.0f); // compute projection matrix

  sp->use(); // activate shading program
  // Send parameters to graphics card
  glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
  glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

  glm::mat4 M = glm::mat4(1.0f);
  glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

  glEnableVertexAttribArray(
      sp->a("vertex")); // Enable sending data to the attribute vertex
  glEnableVertexAttribArray(sp->a("color"));
  glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, myCubeColors);
  glVertexAttribPointer(
      sp->a("vertex"), 4, GL_FLOAT, false, 0,
      myCubeVertices); // Specify source of the data for the attribute vertex

  glDrawArrays(GL_TRIANGLES, 0, myCubeVertexCount); // Draw the object
  glDisableVertexAttribArray(sp->a("color"));
  glDisableVertexAttribArray(sp->a("vertex"));

  // CUBE 2
  glm::mat4 Mp1 = glm::translate(
      M, glm::vec3(-5.0f, 0.0f, -5.0f)); // Planet's model matrix is a sun
  glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mp1));
  glEnableVertexAttribArray(
      sp->a("vertex")); // Enable sending data to the attribute vertex
  glEnableVertexAttribArray(sp->a("color"));
  glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, myCubeColors);
  glVertexAttribPointer(
      sp->a("vertex"), 4, GL_FLOAT, false, 0,
      myCubeVertices); // Specify source of the data for the attribute vertex

  glDrawArrays(GL_TRIANGLES, 0, myCubeVertexCount); // Draw the object
  glDisableVertexAttribArray(sp->a("color"));
  glDisableVertexAttribArray(sp->a("vertex"));
  // CUBE 3
  glm::mat4 Mp2 = glm::translate(
      Mp1, glm::vec3(10.0f, 0.0f, 0.0f)); // Planet's model matrix is a sun
  glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mp2));
  glEnableVertexAttribArray(
      sp->a("vertex")); // Enable sending data to the attribute vertex
  glEnableVertexAttribArray(sp->a("color"));
  glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, myCubeColors);
  glVertexAttribPointer(
      sp->a("vertex"), 4, GL_FLOAT, false, 0,
      myCubeVertices); // Specify source of the data for the attribute vertex

  glDrawArrays(GL_TRIANGLES, 0, myCubeVertexCount); // Draw the object
  glDisableVertexAttribArray(sp->a("color"));
  glDisableVertexAttribArray(sp->a("vertex"));

  glfwSwapBuffers(window); // Copy back buffer to front buffer
}

int main(void) {
  GLFWwindow *window; // Pointer to object that represents the application
                      // window

  glfwSetErrorCallback(
      error_callback); // Register error processing callback procedure

  if (!glfwInit()) { // Initialize GLFW library
    fprintf(stderr, "Can't initialize GLFW.\n");
    exit(EXIT_FAILURE);
  }

  window =
      glfwCreateWindow(1000, 700, "OpenGL", NULL,
                       NULL); // Create a window 500pxx500px titled "OpenGL" and
                              // an OpenGL context associated with it.

  if (!window) // If no window is opened then close the program
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(
      window); // Since this moment OpenGL context corresponding to the window
               // is active and all OpenGL calls will refer to this context.
  glfwSwapInterval(1); // During vsync wait for the first refresh

  GLenum err;
  if ((err = glewInit()) != GLEW_OK) { // Initialize GLEW library
    fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
    //	exit(EXIT_FAILURE);
  }

  initOpenGLProgram(window); // Call initialization procedure

  float angle_x = 0; // current rotation angle of the object, x axis
  float angle_z = 0; // current rotation angle of the object, y axis

  glfwSetTime(0); // Zero the timer
  // Main application loop
  while (!glfwWindowShouldClose(
      window)) // As long as the window shouldnt be closed yet...
  {
    angle_x += move_x * glfwGetTime(); // Add angle by which the object was
                                       // rotated in the previous iteration
    angle_z += move_z * glfwGetTime(); // Add angle by which the object was
    // rotated in the previous iteration

    glfwSetTime(0);

    drawScene(window, angle_x, angle_z, camera_x); // Execute drawing procedure
    glfwPollEvents(); // Process callback procedures corresponding to the events
                      // that took place up to now
  }
  freeOpenGLProgram(window);

  glfwDestroyWindow(window); // Delete OpenGL context and the window.
  glfwTerminate();           // Free GLFW resources
  exit(EXIT_SUCCESS);
}
