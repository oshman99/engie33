# Project definition
cmake_minimum_required(VERSION 3.1)
project(engie33)

# Source files
# hacky way to add stb_image.h, fix!
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries_headers")
    
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp) 
set(SOURCES "${SRC_DIR}/main.cpp" 
            "${SRC_DIR}/shaderClass.cpp" 
            "${SRC_DIR}/shaderClass.h"
            "${SRC_DIR}/cameraClass.h"
            "${SRC_DIR}/cameraClass.cpp"
            "${SRC_DIR}/texture2DLoader.cpp"
            "${SRC_DIR}/texture2DLoader.h")

set(SHADERS 
    vertex.glsl
    fragment.glsl
    vertexLightObject.glsl
    fragmentLightObject.glsl
    fragmentLightSource.glsl)

set(TEXTURES
    container.png
    da_dude.png
    container2.png
    container2_specular.png
    matrix.png)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)            
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/textures)      

# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")

# GLFW
find_package(GLFW3 3.2 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

#OpenGL
find_package(OpenGL REQUIRED)

# GLM
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

#assimp 
find_package(assimp REQUIRED)
message(STATUS "assimp included at ${ASSIMP_INCLUDE_DIRS}")




# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PUBLIC "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC "glad")

#imgui
set(IMGUI_DIR "${LIB_DIR}/DearImGui")
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp) 
add_library("ImGui" STATIC ${IMGUI_SOURCES})
#imgui depends on some OpenGL loading library - glad in this case
target_include_directories("ImGui" PUBLIC ${IMGUI_DIR} "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC "ImGui")

#stb_image
include_directories(${LIB_DIR}/stb_image)
link_directories(${LIB_DIR}/stb_image) 


#to build on windows use mingw64 makefiles(MSYS2 + mingw works 100%)
#mingw64 names glfw library slightly different i suppose
IF(MINGW)
    target_link_libraries(${PROJECT_NAME} PUBLIC "assimp" "ImGui" "GLFW3" "glad" "OpenGL::GL" "${CMAKE_DL_LIBS}")
ELSE(MINGW)
    target_link_libraries(${PROJECT_NAME} PUBLIC "assimp" "ImGui" "glfw" "glad" "OpenGL::GL" "${CMAKE_DL_LIBS}")
ENDIF(MINGW)

#Shader and texture handeling

#shaders. Copy files into shaders folder in build
foreach(shader IN LISTS SHADERS)
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/${shader}
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${SRC_DIR}/shaders/${shader}
                                            ${CMAKE_CURRENT_BINARY_DIR}/shaders/${shader}
    DEPENDS
        ${SRC_DIR}/shaders/${shader}
    COMMENT "Copying shader: ${shader}" VERBATIM
    )
  
  # make sure building target triggers the above custom command
  target_sources(${PROJECT_NAME}
    PRIVATE
      ${CMAKE_CURRENT_BINARY_DIR}/shaders/${shader}
    )  
endforeach()

#texture. Copy files into texures folder in build, same as shaders
foreach(texture IN LISTS TEXTURES)
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/textures/${texture}
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/${texture}
                                              ${CMAKE_CURRENT_BINARY_DIR}/textures/${texture}
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/${texture}
    COMMENT "Copying texture: ${texture}" VERBATIM
    )
  
  target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/textures/${texture}
    )  
endforeach()