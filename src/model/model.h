#ifndef MODEL_H
#define MODEL_H

#include "assimp/mesh.h"
#include "glm/fwd.hpp"
#include "shaderprogram.h"
#include "src/mesh/mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <map>
#include <string>
#include <vector>

GLuint readTexture(const char *filename);

class Model {
  public:
    void loadModel(const std::string &pFile, ShaderProgram *sp);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    // void drawMesh(Mesh *mesh, const glm::mat4 &M);
    void Draw(const glm::mat4 &M, ShaderProgram *sp);

    std::vector<Mesh> meshes;
    ShaderProgram *shaderprogram;
    std::map<std::string, unsigned int> loadedTextures;
    const aiScene *scene;
};

#endif
