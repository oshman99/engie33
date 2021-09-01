
#include <modelClass.h>
unsigned int TextureFromFile(const char *path, const std::string &directory);

Model::Model(std::string const &path){
    loadModel(path);
}

void Model::loadModel(std::string path){
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene){
        std::cout <<"ERROR::ASSIMP" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}
void Model::Draw(Shader &shader)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

void Model::processNode(aiNode *node, const aiScene *scene){
    //process node's meshes
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    //do the same for it's children
    //79 детей у рута, дальше вызывается рекурсия, у детей же детей нету
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene){
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    //process vertex pos, normals, and tex coords
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        //assimp doesnt convert well from glm data, so we need those tempory vectors
        glm::vec3 tmpVector;
        tmpVector.x = mesh->mVertices[i].x;
        tmpVector.y = mesh->mVertices[i].y;
        tmpVector.z = mesh->mVertices[i].z;
        vertex.Postinion = tmpVector;

        tmpVector.x = mesh->mNormals[i].x;
        tmpVector.y = mesh->mNormals[i].y;
        tmpVector.z = mesh->mNormals[i].z;
        vertex.Normal = tmpVector;
        //mesh sometimes can have NO textures
        if(mesh->mTextureCoords[0])
        {
            glm::vec2 tmpTexCoords;
            tmpTexCoords.x = mesh->mTextureCoords[0][i].x;
            tmpTexCoords.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = tmpTexCoords;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    //process faces and their indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    //process materials if there is any
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector <Texture> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector <Texture> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    Mesh processedMesh(vertices, indices, textures);
    return processedMesh;
}

std::vector<Texture> Model::loadMaterialTexture (aiMaterial *mat, aiTextureType type, std::string typeName){
    std::vector<Texture> textures;
    for(unsigned int i; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if(!skip)
        {//if texture hasn't been loaded already, load it
            Texture texture;
            texture.ID = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);//add to loaded textures
        }
        std::string debugShit(str.C_Str());
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const std::string &directory){
    std::string filename = std::string(path);
    filename = directory + '/' + filename;
    
    unsigned int textureID;
    glGenTextures(1, &textureID);
    //загружаем и генерируем текстуру
    int width, height, nrChannels;
    //OpenGL ожидает 0 на оси у внизу изображения, сами изображения устанавивают 0.0 вверху оси у. Эта функция переворачивает ось у
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if(data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        if (nrChannels == 3)
            format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Failed to load texture!" << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}