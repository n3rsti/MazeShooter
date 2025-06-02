#include "camera.h"
#include "GLFW/glfw3.h"
#include "glm/common.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "shaderprogram.h"
#include <GL/glew.h>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>

Camera::Camera(float camera_rotation_limit, float fov, float near_plane,
               float far_plane, float sensitivity, float aspect_ratio)
    : camera_rotation_limit(camera_rotation_limit), fov(fov),
      near_plane(near_plane), far_plane(far_plane), sensitivity(sensitivity),
      aspect_ratio(aspect_ratio) {}

void Camera::cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  x_rotation -= (previous_camera_x - xpos) * sensitivity;
  y_rotation -= (previous_camera_y - ypos) / 100 * sensitivity;

  y_rotation =
      glm::clamp(y_rotation, -camera_rotation_limit, camera_rotation_limit);

  previous_camera_x = xpos;
  previous_camera_y = ypos;

  printf("Cursor position: (%f, %f, %f, %f)\n", xpos, ypos, x_rotation,
         y_rotation);
}

// Create view, perspective matrices based on x, z position coordinates and
// camera rotation
void Camera::updateCamera(float x_pos, float z_pos, ShaderProgram *sp) {
  glm::vec3 eye = glm::vec3(x_pos, 10.0f, z_pos);
  glm::vec3 center = glm::vec3(x_pos, 0.0f, z_pos + 0.2f);
  glm::vec3 relative = center - eye;

  center.y = relative.y * cos(y_rotation) - relative.z * sin(y_rotation);
  center.x =
      relative.x * cos(x_rotation / 100) - relative.z * sin(x_rotation / 100);
  center.z =
      relative.x * sin(x_rotation / 100) + relative.z * cos(x_rotation / 100);

  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

  glm::mat4 V = glm::lookAt(eye, center + eye, up);
  glm::mat4 P = glm::perspective(fov, aspect_ratio, near_plane, far_plane);

  glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
  glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
}
