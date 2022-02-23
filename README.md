Small OpenGL engine. Depends on GLFW, GLAD, GLM, stb_image and assimp. Developed primarly on linux, but can be build with MinGW and MSYS2 on Windows. 
First, make an empty build folder

## To build on linux:
gcc is standart in CMake on unix platforms

cmake ..

cmake --build .

## On Windows

MSYS2 + MinGW:

cmake -G "MinGW Makefiles" ..

cmake --build  .
