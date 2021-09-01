# Project definition
cmake_minimum_required(VERSION 3.1)
project(engie33)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries_headers")
    
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp) 
set(SOURCES "${SRC_DIR}/main.cpp" 
            "${SRC_DIR}/shaderClass.cpp" 
            "${SRC_DIR}/shaderClass.h"
            "${SRC_DIR}/cameraClass.h"
            "${SRC_DIR}/cameraClass.cpp"
            "${SRC_DIR}/meshClass.cpp"
            "${SRC_DIR}/meshClass.h"
            "${SRC_DIR}/modelClass.cpp"
            "${SRC_DIR}/modelClass.h"
            )

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

set(MODELS
    survival_guitar)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)         
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)              
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets/models)   
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



#on-change asset update
set(DATA_IN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(DATA_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/assets)

#textures
file(GLOB_RECURSE TEXTURE_FILES "${DATA_IN_DIR}/textures/*")
foreach(tex_path IN LISTS TEXTURE_FILES)
    cmake_path(GET tex_path FILENAME texture)
    add_custom_command(
        OUTPUT
            ${DATA_OUT_DIR}/textures/${texture}
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different  ${DATA_IN_DIR}/textures/${texture}
                                                ${DATA_OUT_DIR}/textures/${texture}
        DEPENDS
            ${DATA_IN_DIR}/textures/${texture}
        COMMENT "Copying texture: ${texture}" VERBATIM
        )
    
    # make sure building target triggers the above custom command
    target_sources(${PROJECT_NAME}
                    PRIVATE
                    ${DATA_OUT_DIR}/textures/${texture}
    )  
endforeach()

#models
file(GLOB MODEL_DIRECTORIES "${DATA_IN_DIR}/models/*")


foreach(model_dir IN LISTS MODEL_DIRECTORIES)
    #for each model directory get it's name and create same in build dir
    cmake_path(GET model_dir FILENAME model_dir_name) 
    message("Model dir name ${model_dir_name}")
    file(MAKE_DIRECTORY ${DATA_OUT_DIR}/models/${model_dir_name}) 
    set(BIN_MODEL_DIR ${DATA_OUT_DIR}/models/${model_dir_name})
    set(SRC_MODEL_DIR ${DATA_IN_DIR}/models/${model_dir_name})

    #get files in model dir
    file(GLOB_RECURSE MODEL_FILES "${model_dir}/*")
    foreach(model_file_path IN LISTS MODEL_FILES) 

        cmake_path(GET model_file_path FILENAME model_file) 
        add_custom_command(
            OUTPUT
                ${BIN_MODEL_DIR}/${model_file}
            COMMAND
                ${CMAKE_COMMAND} -E copy_if_different  ${SRC_MODEL_DIR}/${model_file}
                                                        ${BIN_MODEL_DIR}/${model_file}
            DEPENDS
                ${SRC_MODEL_DIR}/${model_file}
            COMMENT "Copying model file: ${model_dir_name}/${model_file}" VERBATIM
            )
        
        # make sure building target triggers the above custom command
        target_sources(${PROJECT_NAME}
                        PRIVATE
                        ${BIN_MODEL_DIR}/${model_file}
        )  
        #Hacky. With this flag set, no compilation of the given source file will happen, as if this is header-only file. 
        #Wavefront .obj files have same extension as CMake object file and so they are triggering some rule in linking stage.
        #So it's better to make linker ignore them.
        set_source_files_properties(${BIN_MODEL_DIR}/${model_file} PROPERTIES HEADER_FILE_ONLY ON)
    endforeach()
endforeach()