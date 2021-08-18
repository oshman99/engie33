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
//everything else - meh
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//debbuging
//#include <glm/gtx/string_cast.hpp>

//библиотека для загрузк текстур, инклюдится в texture2DLoader
//#include <stb_image/stb_image.h>

#include <iostream>
#include <shaderClass.h>
#include <texture2DLoader.h>
#include <cameraClass.h>

//callbacks
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//настройки
const unsigned int SCR_WIDTH = 1360;
const unsigned int SCR_HEIGHT = 720;
//сетап камеры
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
//то, куда смотрит камера, аналогично yaw = 90.0f;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//перемнные для настройки плавности движения
float deltaTime = 0.0f;//время между текущим и прошлым кадром
float lastFrame = 0.0f;//время прошлого кадра
//init mouse position variables(center of the screen)
float lastX = SCR_WIDTH/2; 
float lastY = SCR_HEIGHT/2;
//flag to check if this is the first time we recive mouse input
bool firstMouse = true;

Camera camera(glm::vec3(0.0f, 0.0, 3.0f));

//position of objects in worldview
glm::vec3 lightPos(2.0f, 2.0f, -4.0f);
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
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    imgload::loadPNG("textures/container.png");

    //вторая текстура
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    imgload::loadPNG("textures/da_dude.png");

    //анбиндим текстуры
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
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

    //отправляем position attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //отправляем normals attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);    
    //отправялем texture coords attribute
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
    //material struct
    lightObjectShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    lightObjectShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    lightObjectShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    lightObjectShader.setFloat("material.shininess",32.0f);
    //light struct  
    lightObjectShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
    lightObjectShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    lightObjectShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    unsigned int modelLoc = glGetUniformLocation(lightObjectShader.ID, "model");
    unsigned int viewLoc = glGetUniformLocation(lightObjectShader.ID, "view");
    unsigned int projectionLoc = glGetUniformLocation(lightObjectShader.ID, "projection");

    lightSourceShader.use();
    unsigned int modelLocLight = glGetUniformLocation(lightSourceShader.ID, "model");
    unsigned int viewLocLight = glGetUniformLocation(lightSourceShader.ID, "view");
    unsigned int projectionLocLight = glGetUniformLocation(lightSourceShader.ID, "projection");

    //Выбираем способ отрисовки примитивов. Первый прм - приминяем к передней и задней части примитива. Второй - тип отрисовки(п. - GL_LINE/GL_FILL)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //устанавливаем размер и положение Viewport-a OpenGL в окне
    glViewport(0,0, SCR_WIDTH, SCR_HEIGHT);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    //------render loop-------
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        //render commands go here!

        //выбор цвета для очистки экрана (state-setting func)
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //установка бита колор буфера, очистка экрана заданным цветом(state-using func) и депф буфра
        glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
        //sort of telling ImGui that we are working on a new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //обновляем переменные с временем кадров
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


      //биндим текстуры на соотвуствующие текстур юниты
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2); 
        lightObjectShader.use();
        lightObjectShader.setVec3("viewPos",camera.Position.x, camera.Position.y, camera.Position.z);
        //оюновляем view matrix, камера двигается!
        view = camera.GetVewMatrix();  
        //обновляем projection matrix, fov furstum box-a меняется
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        //обновляем юниформы-матрицы
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4(1.0f);
        model =  glm::translate(model, objectPos);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        lightPos = glm::vec3(glm::cos((float)glfwGetTime())*2.0f, 0.1f, glm::sin((float)glfwGetTime())*2.0f);
        lightObjectShader.setVec3("light.position", lightPos);

        lightObjectShader.setVec3("material.ambient",0.1f, 0.1745f, 0.1f);
        lightObjectShader.setVec3("material.diffuse",0.4f, 0.61424f, 0.07568);
        lightObjectShader.setVec3("material.specular", 0.633f, 0.727811f, 0.633f);
        lightObjectShader.setFloat("material.shininess",0.6f * 128.0f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightSourceShader.use();
        glUniformMatrix4fv(viewLocLight, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLocLight, 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4(1.0f);
        model =  glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); 
        glUniformMatrix4fv(modelLocLight, 1, GL_FALSE, glm::value_ptr(model));
        
        lightSourceShader.setVec3("lightColor", lightColor); 
        glBindVertexArray(lightVAO);
        //теперь рендерим свет
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        ImGui::Begin("My name is Imgui window amd i'm an ass to implement blyat!");
        ImGui::Text("Hello there adventurer!");
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

    glfwTerminate();
    return 0;
}

//process key presses 
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    const float cameraSpeed = 2.5f * deltaTime;
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
