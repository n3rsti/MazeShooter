#ifndef MAZE_H
#define MAZE_H

#include "glm/ext/vector_float3.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include <GL/glew.h>
#include <vector>
#include <string>
#include "../../shaderprogram.h"

// Structure for a maze wall
struct Wall {
    glm::vec3 position;
    glm::vec3 size;
    int textureType; // 0 = default, 1 = bricks, 2 = stone
};

// Structure for a following ball
struct Ball {
    glm::vec3 position;
    float radius;
    float followSpeed;
    bool active;
    glm::vec3 color;
    float lastShotTime;
};

class Maze {
private:
    std::vector<Wall> walls;
    std::vector<std::vector<int>> mazeLayout;
    Ball followingBall;
    
    GLuint wallVAO, wallVBO;
    GLuint ballVAO, ballVBO;
    
    // Textures
    GLuint bricksDiffuseTex;
    GLuint bricksNormalTex;
    GLuint bricksHeightTex;
    GLuint bricksSpecularTex;
    
    // Setup vertex data for rendering
    void setupWallVertexData();
    void setupBallVertexData();
    
    // Load textures for the maze
    void loadTextures();
    
public:
    Maze();
    ~Maze();
    
    // Initialize the maze
    void init();
    
    // Generate a simple maze layout
    void generateMaze(int width, int height);
    
    // Get the walls for ray tracing
    const std::vector<Wall>& getWalls() const { return walls; }
    
    // Check collision with walls
    bool checkCollision(const glm::vec3& position, float radius) const;
    
    // Update the following ball position
    void updateBall(const glm::vec3& playerPosition, float deltaTime);
    
    // Try to shoot the ball
    bool shootBall(const glm::vec3& rayOrigin, const glm::vec3& rayDirection);
    
    // Render the maze using traditional rendering
    void render(ShaderProgram* sp, const glm::mat4& V, const glm::mat4& P);
    
    // Get ball position and state
    const Ball& getBall() const { return followingBall; }
    
    // Reset ball to origin
    void resetBall();
};

#endif // MAZE_H