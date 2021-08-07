#include <glad/glad.h>
#include <GLFW/glfw3.h>
//тест подключение библиотеки с смэйком
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//библиотека для загрузк текстур, инклюдится в texture2DLoader
//#include <stb_image/stb_image.h>

#include <iostream>
#include <shaderClass.h>
#include <texture2DLoader.h>
#include <cameraClass.h>

//сетап камеры
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
//то, куда смотрит камера, аналогично yaw = 90.0f;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//перемнные для настройки плавности движения
float deltaTime = 0.0f;//время между текущим и прошлым кадром
float lastFrame = 0.0f;//время прошлого кадра

//process key presses 
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    const float cameraSpeed = 2.5f * deltaTime;//adjust
    //move forward/backwards
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    //strafe
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

//window resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//init mouse position variables(center of the screen)
float lastX = 400, lastY = 300, yaw = -90.0f, pitch = 0.0f;
//flag to check if this is the first time we recive mouse input
bool firstMouse = true;

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

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

float fov = 45.0f;

//scrolling callback(zoom)
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if(fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

int main()
{
    //------init GLFW, set everything and create window and context-------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL!", NULL, NULL);
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
    //функция умножающая на матрицу, которая "сдвигает" окружение как будто камера смторит на объект(1 - позиция камеры, 2 - на что смотреть, 3 - вектор направленный вверх)
    glm::mat4 view;
    //glm, создаем матрицы для перехода в 3д
    //переход в world space. Устанавливаем при рендере
    glm::mat4 model = glm::mat4(1.0f);
    
    //переход в clip space(projection, frustum box). В нашем случае perspective
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(fov), 800.0f/600.0f, 0.1f, 100.0f);
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
    glViewport(0,0, 800, 600);
    
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
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        //обновляем projection matrix, fov furstum box-a меняется
        projection = glm::perspective(glm::radians(fov), 800.0f/600.0f, 0.1f, 100.0f);

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