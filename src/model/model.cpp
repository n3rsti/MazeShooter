#include "model.h"
#include "assimp/Importer.hpp"
#include "assimp/mesh.h"
#include "assimp/postprocess.h"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shaderprogram.h"
#include <assimp/scene.h>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

void Model::loadModel(const std::string &pFile, ShaderProgram *sp,
                      std::vector<GLuint> &textures) {

    this->textures = textures;
    shaderprogram = sp;
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        pFile, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                   aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    if (nullptr == scene) {
        std::cerr << "Error loading model: " << importer.GetErrorString()
                  << std::endl;

        return;
    };

    std::cout << "Model loaded successfully: " << pFile << std::endl;

    if (scene->mNumMeshes == 0) {
        std::cerr << "No meshes found in the model." << std::endl;
        return;
    }

    meshes.reserve(scene->mNumMeshes);
    processNode(scene->mRootNode, scene);

    std::cout << "Number of meshes processed: " << meshes.size() << std::endl;
}

void Model::processNode(aiNode *node, const aiScene *scene) {

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    if (mesh == nullptr) {
        std::cerr << "Null mesh encountered." << std::endl;
        return;
    }

    Mesh meshData;
    meshData.mesh = mesh;
    std::vector<unsigned int> indices;
    std::vector<GLint> texCoordNames;

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace &face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    for (unsigned int i = 0; i < mesh->GetNumUVChannels(); i++) {
        GLint texCoordName =
            shaderprogram->a(("texCoord" + std::to_string(i)).c_str());
        texCoordNames.push_back(texCoordName);
    }

    meshData.indices = indices;
    meshData.texCoordsAttributes = texCoordNames;
    meshes.push_back(meshData);
}

void Model::drawMesh(Mesh *mesh, const glm::mat4 &M) {
    if (mesh == nullptr) {
        std::cerr << "Null mesh encountered during draw." << std::endl;
        return;
    }

    glUniformMatrix4fv(shaderprogram->u("M"), 1, false, glm::value_ptr(M));
    glEnableVertexAttribArray(shaderprogram->a("vertex"));
    glVertexAttribPointer(shaderprogram->a("vertex"), 3, GL_FLOAT, false, 0,
                          mesh->mesh->mVertices);

    std::cout << "Texture coordinates count: "
              << mesh->mesh->mNumUVComponents[0] << std::endl;

    for (unsigned int i = 0; i < mesh->texCoordsAttributes.size(); i++) {
        glEnableVertexAttribArray(mesh->texCoordsAttributes[i]);
        glVertexAttribPointer(mesh->texCoordsAttributes[i], 2, GL_FLOAT, false,
                              0, mesh->mesh->mTextureCoords[0]);

        glDisableVertexAttribArray(mesh->texCoordsAttributes[i]);
    }

    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i + 1);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glUniform1i(
            shaderprogram->u(("textureMap" + std::to_string(i + 1)).c_str()),
            i + 1);
    }

    glEnableVertexAttribArray(shaderprogram->a("normal"));
    glVertexAttribPointer(shaderprogram->a("normal"), 3, GL_FLOAT, false, 0,
                          mesh->mesh->mNormals);

    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT,
                   mesh->indices.data());
    std::cout << "Drawing mesh with " << mesh->mesh->mNumVertices
              << " vertices, " << mesh->indices.size() << " indices"
              << std::endl;

    glDisableVertexAttribArray(shaderprogram->a("vertex"));
    // glDisableVertexAttribArray(sp->a("texCoord2"));
    // glDisableVertexAttribArray(sp->a("texCoord3"));
    glDisableVertexAttribArray(shaderprogram->a("normal"));

    // Here you would typically bind the VAO and draw the mesh using OpenGL
    // For example:
    // glBindVertexArray(mesh->VAO);
    // glDrawElements(GL_TRIANGLES, mesh->mNumFaces * 3, GL_UNSIGNED_INT, 0);
}

void Model::draw(const glm::mat4 &M) {
    for (auto &mesh : meshes) {
        drawMesh(&mesh, M);
    }
}
