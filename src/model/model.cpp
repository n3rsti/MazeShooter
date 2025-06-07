#include "model.h"
#include "assimp/Importer.hpp"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/postprocess.h"
#include "assimp/types.h"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "lodepng.h"
#include "shaderprogram.h"
#include "src/mesh/mesh.h"
#include <assimp/scene.h>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

GLuint readTexture(const char *filename) {
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);

    // Read into computers memory
    std::vector<unsigned char> image; // Allocate memory
    unsigned width, height;           // Variables for image size
    // Read the image
    unsigned error = lodepng::decode(image, width, height, filename);
    if (error) {
        std::cerr << "Error loading texture: " << lodepng_error_text(error)
                  << std::endl;
        return 0; // Return 0 if there was an error
    }

    std::cout << "Texture loaded successfully: " << filename << " (" << width
              << "x" << height << ")" << std::endl;

    // Import to graphics card memory
    glGenTextures(1, &tex);            // Initialize one handle
    glBindTexture(GL_TEXTURE_2D, tex); // Activate handle
    // Copy image to graphics cards memory reprezented by the active handle
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}

void Model::loadModel(const std::string &pFile, ShaderProgram *sp) {
    shaderprogram = sp;
    Assimp::Importer importer;
    scene = importer.ReadFile(
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
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<unsigned int> indices;

    std::vector<GLint> texCoordNames;
    std::vector<GLuint> meshTextures;
    std::vector<Vertex> vertices;

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;

        vertex.Position = vector;
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        if (mesh->mTextureCoords[0]) // does the mesh contain texture
                                     // coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We
            // thus make the assumption that we won't use models where a vertex
            // can have multiple texture coordinates so we always take the first
            // set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        } else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->GetNumUVChannels(); i++) {
        GLint texCoordName =
            shaderprogram->a(("texCoord" + std::to_string(i)).c_str());
        texCoordNames.push_back(texCoordName);
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    aiString path;

    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
        std::string texturePath = path.C_Str();

        // Check if already loaded
        if (loadedTextures.find(texturePath) != loadedTextures.end()) {
            // Already loaded, reuse
            meshTextures.push_back(loadedTextures[texturePath]);
        } else {
            // Not loaded yet, so load and store
            // GLuint textureID = readTexture(texturePath);
            GLuint textureID = readTexture(texturePath.c_str());
            loadedTextures[texturePath] = textureID;
            meshTextures.push_back(textureID);
        }
    }

    return Mesh(vertices, indices, texCoordNames, meshTextures);
}

// void Model::drawMesh(Mesh *mesh, const glm::mat4 &M) {
//
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     if (mesh == nullptr) {
//         std::cerr << "Null mesh encountered during draw." << std::endl;
//         return;
//     }
//
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//     glUniformMatrix4fv(shaderprogram->u("M"), 1, false, glm::value_ptr(M));
//     glEnableVertexAttribArray(shaderprogram->a("vertex"));
//     glVertexAttribPointer(shaderprogram->a("vertex"), 3, GL_FLOAT, false, 0,
//                           mesh->vertices);
//
//     // std::cout << "Texture coordinates count: "
//     //           << mesh->mesh->mNumUVComponents[0] << std::endl;
//
//     for (unsigned int i = 0; i < mesh->texCoordsAttributes.size(); i++) {
//         glEnableVertexAttribArray(mesh->texCoordsAttributes[i]);
//         glVertexAttribPointer(mesh->texCoordsAttributes[i], 2, GL_FLOAT,
//         false,
//                               0, mesh->mesh->mTextureCoords[0]);
//
//         glDisableVertexAttribArray(mesh->texCoordsAttributes[i]);
//     }
//
//     // std::cout << "Number of textures: " << mesh->meshTextures.size()
//     //           << std::endl;
//
//     for (unsigned int i = 0; i < mesh->meshTextures.size(); i++) {
//         glActiveTexture(GL_TEXTURE0 + i);
//         glBindTexture(GL_TEXTURE_2D, mesh->meshTextures[i]);
//         glUniform1i(
//             shaderprogram->u(("textureMap" + std::to_string(i)).c_str()), i);
//     }
//
//     glEnableVertexAttribArray(shaderprogram->a("normal"));
//     glVertexAttribPointer(shaderprogram->a("normal"), 3, GL_FLOAT, false, 0,
//                           mesh->mesh->mNormals);
//
//     glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT,
//                    mesh->indices.data());
//     // std::cout << "Drawing mesh with " << mesh->mesh->mNumVertices
//     //           << " vertices, " << mesh->indices.size() << " indices"
//     //           << std::endl;
//     //
//     glDisableVertexAttribArray(shaderprogram->a("vertex"));
//     // glDisableVertexAttribArray(sp->a("texCoord2"));
//     // glDisableVertexAttribArray(sp->a("texCoord3"));
//     glDisableVertexAttribArray(shaderprogram->a("normal"));
//
//     // Here you would typically bind the VAO and draw the mesh using OpenGL
//     // For example:
//     // glBindVertexArray(mesh->VAO);
//     // glDrawElements(GL_TRIANGLES, mesh->mNumFaces * 3, GL_UNSIGNED_INT, 0);
// }

void Model::Draw(const glm::mat4 &M, ShaderProgram *sp) {
    for (auto &mesh : meshes) {
        mesh.Draw(M, sp);
    }
}
