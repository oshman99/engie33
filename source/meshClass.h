#ifndef MESH_H
#define MESH_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <shaderClass.h>
#include <string>
#include <vector>
struct Vertex
{
    glm::vec3 Postinion;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture
{
    unsigned int ID;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO;
    Mesh (std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader &shader);
private:
    unsigned int VBO, VBA, EBO;
    void setupMesh();
};
#endif