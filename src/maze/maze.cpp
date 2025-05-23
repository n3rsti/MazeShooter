#include "maze.h"
#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include "../../lodepng.h"
#include <GLFW/glfw3.h>

// Vertex data for a unit cube
static const float cubeVertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    // Back face
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    // Left face
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    // Right face
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,

    // Top face
    -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    // Bottom face
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f
};

// Texture coordinates for a cube
static const float cubeTexCoords[] = {
    // Front face
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    // Back face
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    // Left face
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    // Right face
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    // Top face
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,

    // Bottom face
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f
};

// Normals for a cube
static const float cubeNormals[] = {
    // Front face
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    // Back face
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,

    // Left face
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,

    // Right face
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    // Top face
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,

    // Bottom face
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f
};

Maze::Maze() : wallVAO(0), wallVBO(0), ballVAO(0), ballVBO(0),
               bricksDiffuseTex(0), bricksNormalTex(0), bricksHeightTex(0), bricksSpecularTex(0) {
    // Initialize ball with better visibility
    followingBall.position = glm::vec3(3.0f, 1.0f, 3.0f); // Position not at origin, and higher up
    followingBall.radius = 0.7f;                         // Larger size
    followingBall.followSpeed = 2.0f;                    // Faster follow speed
    followingBall.active = true;
    followingBall.color = glm::vec3(1.0f, 0.1f, 0.1f);   // Brighter red
    followingBall.lastShotTime = 0.0f;
}

Maze::~Maze() {
    // Clean up resources
    if (wallVAO != 0) {
        glDeleteVertexArrays(1, &wallVAO);
        glDeleteBuffers(1, &wallVBO);
    }
    
    if (ballVAO != 0) {
        glDeleteVertexArrays(1, &ballVAO);
        glDeleteBuffers(1, &ballVBO);
    }
    
    // Delete textures
    if (bricksDiffuseTex != 0) glDeleteTextures(1, &bricksDiffuseTex);
    if (bricksNormalTex != 0) glDeleteTextures(1, &bricksNormalTex);
    if (bricksHeightTex != 0) glDeleteTextures(1, &bricksHeightTex);
    if (bricksSpecularTex != 0) glDeleteTextures(1, &bricksSpecularTex);
}

void Maze::init() {
    // Load textures
    loadTextures();
    
    // Setup rendering data
    setupWallVertexData();
    setupBallVertexData();
    
    // Generate a small maze
    generateMaze(10, 10);
}

GLuint loadTexture(const char* filename) {
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);

    // Read into computer's memory
    std::vector<unsigned char> image;
    unsigned width, height;
    
    // Read the image
    unsigned error = lodepng::decode(image, width, height, filename);
    if (error) {
        std::cout << "Error loading texture " << filename << ": " << lodepng_error_text(error) << std::endl;
        return 0;
    }

    // Import to graphics card memory
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    // Copy image to graphics card memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
    
    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Apply anisotropic filtering if available
    float aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    if (aniso > 0.0f) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
    }
    
    return tex;
}

void Maze::loadTextures() {
    // Try to load new high-quality brick textures first
    bricksDiffuseTex = loadTexture("static/img/bricks_hd_diffuse.png");
    bricksNormalTex = loadTexture("static/img/bricks_hd_normal.png");
    bricksHeightTex = loadTexture("static/img/bricks_hd_height.png");
    bricksSpecularTex = loadTexture("static/img/bricks_hd_specular.png");

    // Fallback to previous brick textures if new ones are missing
    if (bricksDiffuseTex == 0) bricksDiffuseTex = loadTexture("static/img/bricks2_diffuse.png");
    if (bricksNormalTex == 0) bricksNormalTex = loadTexture("static/img/bricks2_normal.png");
    if (bricksHeightTex == 0) bricksHeightTex = loadTexture("static/img/bricks2_height.png");
    if (bricksSpecularTex == 0) bricksSpecularTex = loadTexture("static/img/bricks3b_specular.png");

    // Alternative brick textures
    GLuint bricks3Diffuse = loadTexture("static/img/bricks3b_diffuse.png");
    GLuint bricks3Specular = loadTexture("static/img/bricks3b_specular.png");

    // If the first set failed, try the alternative
    if (bricksDiffuseTex == 0) bricksDiffuseTex = bricks3Diffuse;
    if (bricksSpecularTex == 0) bricksSpecularTex = bricks3Specular;

    // If still no texture, use fallback
    if (bricksDiffuseTex == 0) {
        bricksDiffuseTex = loadTexture("static/img/stone-wall.png");
    }
}

void Maze::setupWallVertexData() {
    // Create and bind VAO
    glGenVertexArrays(1, &wallVAO);
    glBindVertexArray(wallVAO);
    
    // Create and bind VBO
    glGenBuffers(1, &wallVBO);
    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    
    // Structure to hold all vertex data
    struct VertexData {
        float position[3];
        float texCoord[2];
        float normal[3];
    };
    
    // Create vertex data array
    std::vector<VertexData> vertices;
    vertices.reserve(36); // 6 faces * 2 triangles * 3 vertices
    
    // Fill vertex data
    for (int i = 0; i < 36; ++i) {
        VertexData vertex;
        
        // Position
        vertex.position[0] = cubeVertices[i*3];
        vertex.position[1] = cubeVertices[i*3+1];
        vertex.position[2] = cubeVertices[i*3+2];
        
        // Texture coordinates
        vertex.texCoord[0] = cubeTexCoords[i*2];
        vertex.texCoord[1] = cubeTexCoords[i*2+1];
        
        // Normal
        vertex.normal[0] = cubeNormals[i*3];
        vertex.normal[1] = cubeNormals[i*3+1];
        vertex.normal[2] = cubeNormals[i*3+2];
        
        vertices.push_back(vertex);
    }
    
    // Upload data to GPU
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), vertices.data(), GL_STATIC_DRAW);
    
    // Set up vertex attributes
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
    glEnableVertexAttribArray(0);
    
    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, texCoord));
    glEnableVertexAttribArray(1);
    
    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));
    glEnableVertexAttribArray(2);
    
    // Unbind VAO
    glBindVertexArray(0);
}

void Maze::setupBallVertexData() {
    // Create and bind VAO
    glGenVertexArrays(1, &ballVAO);
    glBindVertexArray(ballVAO);
    
    // Create and bind VBO
    glGenBuffers(1, &ballVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
    
    // Create a sphere using a UV sphere approach
    const int stacks = 20;
    const int slices = 20;
    std::vector<float> vertices;
    
    for (int i = 0; i <= stacks; ++i) {
        float v = i / (float)stacks;
        float phi = v * M_PI;
        
        for (int j = 0; j <= slices; ++j) {
            float u = j / (float)slices;
            float theta = u * 2.0f * M_PI;
            
            // Calculate vertex position
            float x = std::sin(phi) * std::cos(theta);
            float y = std::cos(phi);
            float z = std::sin(phi) * std::sin(theta);
            
            // Position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Normal (same as position for a unit sphere)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Texture coordinates
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }
    
    // Generate indices for triangle strips
    std::vector<unsigned int> indices;
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = (i * (slices + 1)) + j;
            int second = first + slices + 1;
            
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);
            
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    
    // Upload vertex data
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Create and bind index buffer
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Set up vertex attributes
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Unbind VAO
    glBindVertexArray(0);
}

void Maze::generateMaze(int width, int height) {
    // Clear existing walls
    walls.clear();
    
    // Define maze properties
    float wallHeight = 2.0f;
    float cellSize = 3.0f;
    float mazeWidth = width * cellSize;
    float mazeHeight = height * cellSize;
    
    // Create outer boundary walls
    // North wall
    Wall northWall;
    northWall.position = glm::vec3(0.0f, wallHeight / 2.0f - 1.0f, -mazeHeight/2.0f);
    northWall.size = glm::vec3(mazeWidth/2.0f, wallHeight / 2.0f, 0.5f);
    northWall.textureType = 1; // Brick texture
    walls.push_back(northWall);
    
    // South wall
    Wall southWall;
    southWall.position = glm::vec3(0.0f, wallHeight / 2.0f - 1.0f, mazeHeight/2.0f);
    southWall.size = glm::vec3(mazeWidth/2.0f, wallHeight / 2.0f, 0.5f);
    southWall.textureType = 1; // Brick texture
    walls.push_back(southWall);
    
    // West wall
    Wall westWall;
    westWall.position = glm::vec3(-mazeWidth/2.0f, wallHeight / 2.0f - 1.0f, 0.0f);
    westWall.size = glm::vec3(0.5f, wallHeight / 2.0f, mazeHeight/2.0f);
    westWall.textureType = 1; // Brick texture
    walls.push_back(westWall);
    
    // East wall
    Wall eastWall;
    eastWall.position = glm::vec3(mazeWidth/2.0f, wallHeight / 2.0f - 1.0f, 0.0f);
    eastWall.size = glm::vec3(0.5f, wallHeight / 2.0f, mazeHeight/2.0f);
    eastWall.textureType = 1; // Brick texture
    walls.push_back(eastWall);
    
    // Add internal maze walls - a more structured pattern
    
    // First vertical wall near center
    Wall wall1;
    wall1.position = glm::vec3(-cellSize, wallHeight / 2.0f - 1.0f, 0.0f);
    wall1.size = glm::vec3(0.5f, wallHeight / 2.0f, mazeHeight/2.0f - cellSize);
    wall1.textureType = 1;
    walls.push_back(wall1);
    
    // Second vertical wall
    Wall wall2;
    wall2.position = glm::vec3(cellSize * 2, wallHeight / 2.0f - 1.0f, -cellSize);
    wall2.size = glm::vec3(0.5f, wallHeight / 2.0f, mazeHeight/2.0f - cellSize);
    wall2.textureType = 1;
    walls.push_back(wall2);
    
    // First horizontal wall
    Wall wall3;
    wall3.position = glm::vec3(0.0f, wallHeight / 2.0f - 1.0f, -cellSize * 2);
    wall3.size = glm::vec3(mazeWidth/2.0f - cellSize * 2, wallHeight / 2.0f, 0.5f);
    wall3.textureType = 1;
    walls.push_back(wall3);
    
    // Second horizontal wall
    Wall wall4;
    wall4.position = glm::vec3(0.0f, wallHeight / 2.0f - 1.0f, cellSize * 2);
    wall4.size = glm::vec3(mazeWidth/2.0f - cellSize, wallHeight / 2.0f, 0.5f);
    wall4.textureType = 1;
    walls.push_back(wall4);
    
    // Add some room-like structures
    
    // Room 1
    Wall room1Wall1;
    room1Wall1.position = glm::vec3(-mazeWidth/2.0f + cellSize * 2, wallHeight / 2.0f - 1.0f, -mazeHeight/2.0f + cellSize * 2);
    room1Wall1.size = glm::vec3(cellSize, wallHeight / 2.0f, 0.5f);
    room1Wall1.textureType = 1;
    walls.push_back(room1Wall1);
    
    Wall room1Wall2;
    room1Wall2.position = glm::vec3(-mazeWidth/2.0f + cellSize * 2, wallHeight / 2.0f - 1.0f, -mazeHeight/2.0f + cellSize);
    room1Wall2.size = glm::vec3(0.5f, wallHeight / 2.0f, cellSize);
    room1Wall2.textureType = 1;
    walls.push_back(room1Wall2);
    
    // Room 2
    Wall room2Wall1;
    room2Wall1.position = glm::vec3(mazeWidth/2.0f - cellSize * 2, wallHeight / 2.0f - 1.0f, mazeHeight/2.0f - cellSize * 2);
    room2Wall1.size = glm::vec3(cellSize, wallHeight / 2.0f, 0.5f);
    room2Wall1.textureType = 1;
    walls.push_back(room2Wall1);
    
    Wall room2Wall2;
    room2Wall2.position = glm::vec3(mazeWidth/2.0f - cellSize, wallHeight / 2.0f - 1.0f, mazeHeight/2.0f - cellSize * 2);
    room2Wall2.size = glm::vec3(0.5f, wallHeight / 2.0f, cellSize);
    room2Wall2.textureType = 1;
    walls.push_back(room2Wall2);
    
    // Reset the ball position to a visible location near the player start
    resetBall();
}

bool Maze::checkCollision(const glm::vec3& position, float radius) const {
    // Super simplified collision - just check basic distance
    for (const auto& wall : walls) {
        // Skip Y axis check for simplicity
        float dx = std::abs(position.x - wall.position.x);
        float dz = std::abs(position.z - wall.position.z);
        
        // Check if close to the wall (cheaper approximation)
        if (dx < (wall.size.x + radius) && dz < (wall.size.z + radius)) {
            return true;
        }
    }
    
    return false;
}

void Maze::updateBall(const glm::vec3& playerPosition, float deltaTime) {
    if (!followingBall.active) {
        // If the ball is inactive for 2 seconds, respawn it
        float currentTime = glfwGetTime();
        if (currentTime - followingBall.lastShotTime > 2.0f) {
            resetBall();
        }
        return;
    }
    
    // Ball hovers above ground for better visibility
    followingBall.position.y = followingBall.radius + 0.5f + 0.2f * sin(glfwGetTime() * 2.0f); // Add bobbing motion
    
    // Improved following behavior
    float moveSpeed = followingBall.followSpeed * deltaTime;
    
    // Vector toward player
    glm::vec3 direction = playerPosition - followingBall.position;
    direction.y = 0; // Ignore height difference for movement
    
    // Distance-based behavior
    float distance = glm::length(direction);
    
    // Normalize direction vector
    if (distance > 0.1f) {
        direction = direction / distance;
    }
    
    // Move toward player but maintain a distance of 2.5 units
    if (distance > 3.0f) {
        followingBall.position.x += direction.x * moveSpeed;
        followingBall.position.z += direction.z * moveSpeed;
    } else if (distance < 2.0f) {
        // Move away if too close
        followingBall.position.x -= direction.x * moveSpeed * 0.5f;
        followingBall.position.z -= direction.z * moveSpeed * 0.5f;
    }
    
    // Add a small circular motion to make it more noticeable
    float circleRadius = 0.3f;
    float circleSpeed = 2.0f;
    followingBall.position.x += circleRadius * sin(glfwGetTime() * circleSpeed) * deltaTime;
    followingBall.position.z += circleRadius * cos(glfwGetTime() * circleSpeed) * deltaTime;
}

bool Maze::shootBall(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) {
    if (!followingBall.active) {
        return false;
    }
    
    // Improved hit detection with ray-sphere intersection
    glm::vec3 ballDir = followingBall.position - rayOrigin;
    float distance = glm::length(ballDir);
    
    // First check - is the ball within reasonable range and roughly in front of us?
    if (distance < 10.0f && glm::dot(glm::normalize(ballDir), rayDirection) > 0.7f) {
        // More precise check with ray-sphere intersection
        glm::vec3 L = followingBall.position - rayOrigin;
        float tca = glm::dot(L, rayDirection);
        
        // If the center of the sphere is behind the ray, no hit
        if (tca < 0) return false;
        
        float d2 = glm::dot(L, L) - tca * tca;
        float radius2 = followingBall.radius * followingBall.radius;
        
        // If the ray doesn't pass close enough to the sphere center, no hit
        if (d2 > radius2) return false;
        
        // We've hit the ball
        followingBall.active = false;
        followingBall.lastShotTime = glfwGetTime();
        printf("Ball hit! Score +1\n");
        return true;
    }
    
    return false;
}

void Maze::resetBall() {
    // Reset ball to a visible position in front of player
    followingBall.position = glm::vec3(3.0f, followingBall.radius + 0.5f, 3.0f);
    followingBall.active = true;
    followingBall.lastShotTime = glfwGetTime();
}

void Maze::render(ShaderProgram* sp, const glm::mat4& V, const glm::mat4& P) {
    // Bind shader
    glUniformMatrix4fv(sp->u("V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("P"), 1, GL_FALSE, glm::value_ptr(P));
    
    // Prepare for rendering walls
    glBindVertexArray(wallVAO);
    
    // Set brick texture (only once)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bricksDiffuseTex);
    glUniform1i(sp->u("textureMap0"), 0);
    
    // Render walls (no culling for simplicity)
    for (const auto& wall : walls) {
        // Create model matrix
        glm::mat4 M = glm::mat4(1.0f);
        M = glm::translate(M, wall.position);
        M = glm::scale(M, wall.size * 2.0f); // Double size as base cube is -0.5 to 0.5
        
        // Set model matrix
        glUniformMatrix4fv(sp->u("M"), 1, GL_FALSE, glm::value_ptr(M));
        
        // Draw cube
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    
    // Render ball if active
    if (followingBall.active) {
        glBindVertexArray(ballVAO);
        
        // Create model matrix for ball
        glm::mat4 M = glm::mat4(1.0f);
        M = glm::translate(M, followingBall.position);
        M = glm::scale(M, glm::vec3(followingBall.radius));
        
        // Set model matrix
        glUniformMatrix4fv(sp->u("M"), 1, GL_FALSE, glm::value_ptr(M));
        
        // Set ball color
        glUniform3fv(sp->u("ballColor"), 1, glm::value_ptr(followingBall.color));
        
        // Apply emission effect for better visibility
        glUniform1f(sp->u("emission"), 0.5f);
        
        // Draw ball
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // Reset emission
        glUniform1f(sp->u("emission"), 0.0f);
    }
}