#ifndef MODEL_H
#define MODEL_H
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <meshClass.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <iostream>
#include <vector>

class Model
{   
public:
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;  
    Model(std::string const &path);
    void Draw(Shader &shader);
private:

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTexture (aiMaterial *mat, aiTextureType type, std::string typeName); 
};

#endif