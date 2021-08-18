#ifndef TEXTURE2D_LOAD_H
#define TEXTURE2D_LOAD_H
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

//should really add other types!
namespace imgload
{
    //only .png: load *filename* texture to current GL_TEXTURE_2D and generate mipmap
    void loadPNG(const char *filename);
}
#endif