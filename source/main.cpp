//explicitly define GLAD loader to imgui
//#define IMGUI_IMPL_OPENGL_LOADER_GLAD

//imgui - fisrt
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
//glad - second
#include <glad/glad.h>
//GLFW - third
#include <GLFW/glfw3.h>
//everything else - whatever
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
//debbuging
//#include <glm/gtx/string_cast.hpp>

//библиотека для загрузк текстур, инклюдится в texture2DLoader
//#include <stb_image/stb_image.h>

#include <iostream>
#include <shaderClass.h>
#include <texture2DLoader.h>//TODO::remove this abomination, simple function is enough
#include <cameraClass.h>

//callbacks
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//assimp test
 Assimp::Importer importer;
 
//настройки
const unsigned int SCR_WIDTH = 1360;
const unsigned int SCR_HEIGHT = 720;

//камера
Camera camera(glm::vec3(0.0f, 0.0, 3.0f));
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
//то, куда смотрит камера, аналогично yaw = 90.0f;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//init mouse position variables(center of the screen)
float lastX = SCR_WIDTH/2; 
float lastY = SCR_HEIGHT/2;
bool firstMouse = true;

//тайминг кадров
float deltaTime = 0.0f;//время между текущим и прошлым кадром
float lastFrame = 0.0f;//время прошлого кадра

//position of objects in worldview
glm::vec3 pointLightPositions[] = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f)
};
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};
glm::vec3 objectPos(0.0f);
glm::vec3 lightColor(1.0f);

int main()
{
    //some ImGui dependencies crap, no need apperently
    //gladLoadGL();
    //------init GLFW, set everything and create window and context-------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn OpenGL!", NULL, NULL);
    if (window == NULL)
    {
        std::cout <<"Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    //загружаем функции OpenGL через GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD!" << std::endl;
        return -1;
    }
    //-----------------

    //набор вершин для тестирования
    // TODO: написать функцию для организации этого всего, добавить использование EBO
    //сейчас это вершины куба
    float vertices2[] = {
    /*position coords     normals             tex coords*/
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
 
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
 
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f
};


    //класс для создание shader program из vertex и fragment шейдера, путь относительно билд папки
    //TODO: Сделать более удобный доступ к шейдеру, может создать строку с путем в папку, что бы указывать только имя
    //Shader ourShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Shader lightObjectShader("shaders/vertexLightObject.glsl", "shaders/fragmentLightObject.glsl");
    Shader lightSourceShader("shaders/vertexLightObject.glsl", "shaders/fragmentLightSource.glsl");
    //------------------------

    //--------Genereating textures--------
    unsigned int diffuseMap;
    glGenTextures(1, &diffuseMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    imgload::loadPNG("textures/container2.png");

    //2 текстура
    unsigned int specularMap;
    glGenTextures(1, &specularMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    imgload::loadPNG("textures/container2_specular.png");

    //3 текстура
    unsigned int emissionMap;
    glGenTextures(1, &emissionMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissionMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    imgload::loadPNG("textures/matrix.png");

    //анбиндим текстуры
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    //--------------------------

    //------Creating vertex attributes and storing them in VAO------
    //создаем ID Vertex Array Object
    unsigned int VAO, lightVAO;
    //создаем ID Vertex Buffer Object для вершин.
    unsigned int VBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    //position attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //normals attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);    
    //texture coords attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);  

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData - уже вызывали, все настроено
    //отправляем position attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    //-----------------
    
    //включаем depth buffer и соотвественно depth testing
    glEnable(GL_DEPTH_TEST);  

    //регистрация callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //переход в view space(camera)
    //матрицa, которая "сдвигает" окружение как будто камера смторит на объект
    glm::mat4 view;
    //glm, создаем матрицы для перехода в 3д
    //переход в world space. Устанавливаем при рендере
    glm::mat4 model = glm::mat4(1.0f);
    //переход в clip space(projection, frustum box). В нашем случае perspective
    glm::mat4 projection;


    lightObjectShader.use();
    unsigned int modelLoc = glGetUniformLocation(lightObjectShader.ID, "model");
    unsigned int viewLoc = glGetUniformLocation(lightObjectShader.ID, "view");
    unsigned int projectionLoc = glGetUniformLocation(lightObjectShader.ID, "projection");
    lightObjectShader.setInt("material.diffuse", 0);
    lightObjectShader.setInt("material.specular", 1);
    lightObjectShader.setInt("material.emission", 2);

    lightSourceShader.use();
    unsigned int modelLocLight = glGetUniformLocation(lightSourceShader.ID, "model");
    unsigned int viewLocLight = glGetUniformLocation(lightSourceShader.ID, "view");
    unsigned int projectionLocLight = glGetUniformLocation(lightSourceShader.ID, "projection");

    //Выбираем способ отрисовки примитивов. Первый прм - приминяем к передней и задней части примитива. Второй - тип отрисовки(п. - GL_LINE/GL_FILL)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //устанавливаем размер и положение Viewport-a OpenGL в окне
    glViewport(0,0, SCR_WIDTH, SCR_HEIGHT);

    float colorData[] = {1.0, 1.0, 1.0};
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");



    //------render loop-------
    while(!glfwWindowShouldClose(window))
    {   
        //----вычисления для текщего цикла рендера----
        float currentTime = glfwGetTime();
        //обновляем промежуток между кадрами
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
        //матрицы для вычсления перспективы относительно камеры
        view = camera.GetVewMatrix();  
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        
        pointLightPositions[3] = glm::vec3(glm::cos((float)glfwGetTime())*2.0f, 0.1f, glm::sin((float)glfwGetTime())*2.0f);
        //-----------------------------------------------

        processInput(window);

        //выбор цвета для очистки экрана (state-setting func)
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //установка бита колор буфера, очистка экрана заданным цветом(state-using func) и депф буфра
        glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

        //sort of telling ImGui that we are working on a new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //----основной рендер обжект---
        //---обновляем юниформы и текстуры шейдера объекта---
        lightObjectShader.use();
        //material
        //биндим текстуры на соотвуствующие текстур юниты
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emissionMap);
        lightObjectShader.setFloat("material.shininess",32.0f);
        //lights
        //directional light
        lightObjectShader.setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
        lightObjectShader.setVec3("directionalLight.ambient", 0.1f, 0.1f, 0.1f);
        lightObjectShader.setVec3("directionalLight.diffuse", lightColor);
        lightObjectShader.setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);

        //point light 1
        lightObjectShader.setVec3("pointLight[0].position", pointLightPositions[0]);
        lightObjectShader.setVec3("pointLight[0].ambient", 0.05f, 0.05f, 0.05f);
        lightObjectShader.setVec3("pointLight[0].diffuse", lightColor);
        lightObjectShader.setVec3("pointLight[0].specular", 1.0f, 1.0f, 1.0f);
        lightObjectShader.setFloat("pointLight[0].constant",1.0f);
        lightObjectShader.setFloat("pointLight[0].linear",0.09f);
        lightObjectShader.setFloat("pointLight[0].quadratic",0.032f); 
        //point light 2
        lightObjectShader.setVec3("pointLight[1].position", pointLightPositions[1]);
        lightObjectShader.setVec3("pointLight[1].ambient", 0.05f, 0.05f, 0.05f);
        lightObjectShader.setVec3("pointLight[1].diffuse", lightColor);
        lightObjectShader.setVec3("pointLight[1].specular", 1.0f, 1.0f, 1.0f);
        lightObjectShader.setFloat("pointLight[1].constant",1.0f);
        lightObjectShader.setFloat("pointLight[1].linear",0.09f);
        lightObjectShader.setFloat("pointLight[1].quadratic",0.032f); 
        //point light 3
        lightObjectShader.setVec3("pointLight[2].position", pointLightPositions[2]);
        lightObjectShader.setVec3("pointLight[2].ambient", 0.05f, 0.05f, 0.05f);
        lightObjectShader.setVec3("pointLight[2].diffuse", lightColor);
        lightObjectShader.setVec3("pointLight[2].specular", 1.0f, 1.0f, 1.0f);
        lightObjectShader.setFloat("pointLight[2].constant",1.0f);
        lightObjectShader.setFloat("pointLight[2].linear",0.09f);
        lightObjectShader.setFloat("pointLight[2].quadratic",0.032f); 
        //point light 4
        lightObjectShader.setVec3("pointLight[3].position", pointLightPositions[3]);
        lightObjectShader.setVec3("pointLight[3].ambient", 0.05f, 0.05f, 0.05f);
        lightObjectShader.setVec3("pointLight[3].diffuse", lightColor);
        lightObjectShader.setVec3("pointLight[3].specular", 1.0f, 1.0f, 1.0f);
        lightObjectShader.setFloat("pointLight[3].constant",1.0f);
        lightObjectShader.setFloat("pointLight[3].linear",0.09f);
        lightObjectShader.setFloat("pointLight[3].quadratic",0.03f); 

        //spotlight
        lightObjectShader.setVec3("spotLight.position", camera.Position);
        lightObjectShader.setVec3("spotLight.direction", camera.Front);
        lightObjectShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightObjectShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightObjectShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightObjectShader.setFloat("spotLight.constant", 1.0f);
        lightObjectShader.setFloat("spotLight.linear", 0.09);
        lightObjectShader.setFloat("spotLight.quadratic", 0.03);
        lightObjectShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightObjectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f))); 
        //передаем текущее время
        lightObjectShader.setFloat("u_time", currentTime);
        lightObjectShader.setVec3("viewPos", camera.Position);
        //оюновляем матрицы проекции (типа?)
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        for(unsigned int i = 0; i < 10; i++)
        {
            model = glm::mat4(1.0f);
            model =  glm::translate(model, cubePositions[i]);
            float angle = 20.0f *i;
            model = glm::rotate(model, glm::radians(angle),glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        //---рендер лампы-куба----
        //---обновляем юниформы шейдера освещения---
        lightSourceShader.use();
        lightSourceShader.setVec3("lightColor", lightColor); 
        glUniformMatrix4fv(viewLocLight, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLocLight, 1, GL_FALSE, glm::value_ptr(projection));
        for(unsigned int i = 0; i < 4; i++)
        {
            model =  glm::mat4(1.0f);
            model =  glm::translate(model, pointLightPositions[i]);
            model =  glm::scale(model, glm::vec3(0.2f)); 
            glUniformMatrix4fv(modelLocLight, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        //ImGui рендер
        //TODO::Создавать меню по нажатию кнопки и потом обрабатывать инпут. Возможно надо перенести на другой поток
        ImGui::Begin("My name is Imgui window amd i'm an ass to implement blyat!");
        ImGui::Text("Hello there adventurer!");
        ImGui::ColorEdit4("Color", colorData);
        lightColor = glm::make_vec3(colorData);
        ImGui::End();

        //render the ui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        //проверяет сработало ли какое то событие, вызывает callback funtions из стека вроде(?) и обновляет состояние окна
        glfwPollEvents();
    }
    //---------------

    //destroy ImGui context
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //deallocate objects(is it needed?)
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

//process key presses 
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //move forward/backwards
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    //strafe
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

//window resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//mouse movement callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

//scrolling callback(zoom)
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
