#ifndef MODEL_H
#define MODEL_H

#include "assimp/mesh.h"
#include "glm/fwd.hpp"
#include "shaderprogram.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <map>
#include <string>
#include <vector>

GLuint readTexture(const char *filename);

struct Mesh {
    aiMesh *mesh;
    std::vector<unsigned int> indices;
    std::vector<GLint> texCoordsAttributes;
    std::vector<GLuint> meshTextures;
};

class Model {
  public:
    void loadModel(const std::string &pFile, ShaderProgram *sp);
    void processNode(aiNode *node, const aiScene *scene);
    void processMesh(aiMesh *mesh, const aiScene *scene);
    void drawMesh(Mesh *mesh, const glm::mat4 &M);
    void draw(const glm::mat4 &M);

    std::vector<Mesh> meshes;
    ShaderProgram *shaderprogram;
    std::map<std::string, unsigned int> loadedTextures;
    const aiScene *scene;
};

#endif
