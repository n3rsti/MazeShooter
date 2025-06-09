#ifndef CAMERA_H
#define CAMERA_H

#include "glm/fwd.hpp"
#include "shaderprogram.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
  public:
    float camera_rotation_limit;
    float fov;
    float near_plane;
    float far_plane;
    float sensitivity;
    float aspect_ratio;
    float x_rotation = 0;
    float y_rotation = 0;
    float previous_camera_x = 0;
    float previous_camera_y = 0;

    Camera(float camera_rotation_limit, float fov, float near_plane,
           float far_plane, float sensitivity, float aspect_ratio);

    void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

    // Create view, perspective matrices based on x, z position coordinates and
    // camera rotation
    void updateCamera(float x_pos, float z_pos, ShaderProgram *sp, glm::mat4 &V,
                      glm::mat4 &P);

    glm::vec3 getForward() const;
};

#endif // CAMERA_H
