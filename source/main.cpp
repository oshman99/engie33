#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//debbuging
#include <glm/gtx/string_cast.hpp>

//библиотека для загрузк текстур, инклюдится в texture2DLoader
//#include <stb_image/stb_image.h>

#include <iostream>
#include <shaderClass.h>
#include <texture2DLoader.h>
#include <cameraClass.h>

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//настройки
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
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

int main()
{
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
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

    //расположение кучки кубов в world view
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

/*     //debbuging
    glm::vec3 Position = glm::vec3(1.0f, 2.0f, 3.0f);
    glm::mat4 trans(1.0f);
    trans = glm::translate(trans, -Position);
    //glm::translate(trans, -Position);
    std::cout <<  glm::to_string(trans) << std::endl; */

    //класс для создание shader program из vertex и fragment шейдера, путь относительно билд папки
    //TODO: Сделать более удобный доступ к шейдеру, может создать строку с путем в папку, что бы указывать только имя
    Shader ourShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    //------------------------

    //--------Genereating textures--------
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    //настроиваем враппинг и фильтеринг текстуры(точнее типа который щас забиндин)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //загружаем и генерируем текстуру
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
    unsigned int VAO2;
    //создаем ID Vertex Buffer Object для вершин.
    unsigned int VBO2;

    //генерируем VAO и VBO, у них будет ID выше
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    
    glGenBuffers(1, &VBO2);
    //связываем VAO и VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    //отправляем position attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //отправялем texture coords attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);  
    glBindVertexArray(0);

    //-----------------
    
    //включаем depth buffer и соотвественно depth testing
    glEnable(GL_DEPTH_TEST);  
    //регистрация callback function, вызывающаяся когда изменяют размер окна. Эти функции могут вызываться при очень разных обстоятельствах
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //тоже, но движение мыши
    glfwSetCursorPosCallback(window, mouse_callback);
    //тоже, но scrolling
    glfwSetScrollCallback(window, scroll_callback);

    //переход в view space(camera)
    //матрицa, которая "сдвигает" окружение как будто камера смторит на объект
    glm::mat4 view;
    //glm, создаем матрицы для перехода в 3д
    //переход в world space. Устанавливаем при рендере
    glm::mat4 model = glm::mat4(1.0f);
    //переход в clip space(projection, frustum box). В нашем случае perspective
    glm::mat4 projection;

    //setting texture unforms
    ourShader.use();
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    ourShader.setInt("texture2", 1);
    //получаем локэйшон наших матриц
    unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
    unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
    unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");

    //Выбираем способ отрисовки примитивов. Первый прм - приминяем к передней и задней части примитива. Второй - тип отрисовки(п. - GL_LINE/GL_FILL)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //устанавливаем размер и положение Viewport-a OpenGL в окне
    glViewport(0,0, SCR_WIDTH, SCR_HEIGHT);
    
    //------render loop-------
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        //render commands go here!

        //выбор цвета для очистки экрана (state-setting func)
        glClearColor(0.4f, 0.3f, 0.3f, 1.0f);
        //установка бита колор буфера, очистка экрана заданным цветом(state-using func) и депф буфра
        glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

        //биндим текстуры на соотвуствующие текстур юниты
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        //биндим VAO
        glBindVertexArray(VAO2);
        float offset = 0.0;
        ourShader.setFloat("offset", offset);
        //обновляем переменные с временем кадров
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //оюновляем view matrix, камера двигается!
        //(1 - позиция камеры, 2 - на что смотреть, 3 - вектор направленный вверх)
        view = camera.GetVewMatrix();
        //обновляем projection matrix, fov furstum box-a меняется
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);

        //обновляем model matrix что бы кубы раскидались по world view
        for(unsigned int i = 0; i< 10; i++)
        {
            model = glm::mat4(1.0f);
            model =  glm::translate(model, cubePositions[i]);
            float angle;
            if (i%3 == 0)
                angle = (float)glfwGetTime()*10.0f;
            else
                angle = 50.0f;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 3.0f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            //теперь рендерим
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        //обновляем юниформы-матрицы
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        //анбиндим VAO
        glBindVertexArray(0);
        //свапает бэк и фронт баферы по сути
        glfwSwapBuffers(window);
        //проверяет сработало ли какое то событие, вызывает callback funtions из стека вроде(?) и обновляет состояние окна
        glfwPollEvents();
    }
    //---------------

    //удалить окно и все что с ним связано
    glfwTerminate();
    return 0;
}

//process key presses 
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    const float cameraSpeed = 2.5f * deltaTime;//adjust
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
