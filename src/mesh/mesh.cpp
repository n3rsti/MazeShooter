#include "mesh.h"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shaderprogram.h"
#include <cstddef>
#include <string>
#include <vector>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<GLint> texCoordsAttributes,
           std::vector<GLuint> meshTextures) {
    this->vertices = vertices;
    this->indices = indices;
    this->texCoordsAttributes = texCoordsAttributes;
    this->meshTextures = meshTextures;

    setupMesh();
};

void Mesh::Draw(const glm::mat4 &M, ShaderProgram *sp) {

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
    glBindVertexArray(VAO);
    // glUniform3f(sp->u("kd"), material.kd.r, material.kd.g, material.kd.b);
    // glUniform3f(sp->u("ks"), material.ks.r, material.ks.g, material.ks.b);
    // glUniform1f(sp->u("ns"), material.ns);
    // glUniform1i(sp->u("hasTexture"), 0);

    for (unsigned int i = 0; i < meshTextures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, meshTextures[i]);
        glUniform1i(sp->u(("textureMap" + std::to_string(i)).c_str()), i);
    }

    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()),
                   GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() {
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Bitangent));
    // ids
    // glEnableVertexAttribArray(5);
    // glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex),
    //                        (void *)offsetof(Vertex, m_BoneIDs));
    //
    // // weights
    // glEnableVertexAttribArray(6);
    // glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
    //                       (void *)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
};
