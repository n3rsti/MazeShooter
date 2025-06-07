#ifndef MESH_H
#define MESH_H

#include "assimp/types.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "shaderprogram.h"
#include <vector>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

class Mesh {
  public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<GLint> texCoordsAttributes;
    std::vector<GLuint> meshTextures;

    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
         std::vector<GLint> texCoordsAttributes,
         std::vector<GLuint> meshTextures);

    void Draw(const glm::mat4 &M, ShaderProgram *sp);

  private:
    unsigned int VBO, EBO;

    void setupMesh();
};

#endif
