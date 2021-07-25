#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    //program ID
    unsigned int ID;

    //constructor reads and build the shader program
    Shader(const char* vertexPath, const char* fragmentPath);
    //use/activate the shader
    void use();
    //some utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
};

#endif