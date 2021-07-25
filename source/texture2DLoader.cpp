#define STB_IMAGE_IMPLEMENTATION
#include <texture2DLoader.h>


void imgload::loadPNG(const char *filename)
{
    //загружаем и генерируем текстуру
    int width, height, nrChannels;
    //OpenGL ожидает 0 на оси у внизу изображения, сами изображения устанавивают 0.0 вверху оси у. Эта функция переворачивает ось у
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture!" << std::endl;
    }
    stbi_image_free(data);
}