#include <glad/glad.h>
#include <GLFW/glfw3.h>
//тест подключение библиотеки с смэйком
#include <glm/mat2x2.hpp>

//библиотека для загрузк текстур, инклюдится в texture2DLoader
//#include <stb_image/stb_image.h>

#include <iostream>
#include <shaderClass.h>
#include <texture2DLoader.h>

//process key presses 
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

//window resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    //------init GLFW-------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //for MacOS only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //--------

    //--------set everything and create window and context---------
    GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL!", NULL, NULL);
    if (window == NULL)
    {
        std::cout <<"Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    //!!!загружаем функции OpenGL через GLAD!!!
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD!" << std::endl;
        return -1;
    }
    //-----------------

    //вершины нашего первого треугольника!
    float vertices1[] = {
        0.1f,  0.1f, 0.0f,
        0.3f,  0.3f, 0.0f,
        0.1f,  0.3f, 0.0f
    };

    //набор вершин для тестирования
    float vertices2[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

    /*Вершины прямоугольника, используем EBO(Element buffer object) - с его помощью
    вместо повторнго объявления врешин указываем индексы */
    float vertices_rec[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    //массив номеров вершин, начинаем с 0!
    unsigned int indices[] = {
         0, 1, 3,   // first triangle
         1, 2, 3    // second triangle
    };

    //тестим класс для файлов шейдеров
    //шейдеры копируются cmake в билд папку если шо
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
        int width, height, nrChannels;
        //OpenGL ожидает 0 на оси у внизу изображения, сами изображения устанавивают 0.0 вверху оси у. Эта функция переворачивает ось у
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);
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

    //вторая текстура
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    //настроиваем враппинг и фильтеринг текстуры(точнее типа который щас забиндин)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //загружаем и генерируем текстуру
    imgload::loadPNG("textures/da_dude.png");
    //анбиндим
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    //третья текстурка, используем texture2DLoader функцию
    //--------------------------

    //------Creating vertex attributes and storing them in VAO------
    /*Создаем Vertex Array Object(VAO) - он позолит легко и быстро получать доступ к atrributes и VBO.
    Обычно когда в программе есть несколько объектов для отрисовки, сначала создаются
    и настраиваются все VAO(и соотвественно все VBO и attribute pointers), и сохраняются
    для использования далее. Когда нужно нарисовать один из объектов, нужный VAO биндится,
    отрисоывается объект и VAO снова анбиндится*/
    unsigned int VAO, VAO2;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    //создаем Vertex Buffer Object для вершин.
    //Сначала как обычный буфер с таким-то ID
    unsigned int VBO, VBO2;
    glGenBuffers(1, &VBO);

    /*Затем биндим буфер к VBO-типу буфера GL_ARRAY_BUFFER
    OpenGL позволяет биндить множество буферов, 
    но только если они будут разного типа*/
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /*!!!С этого момента любой вызов буфера типа GL_ARRAY_TARGET 
    будет изменять текущий буфер, забиндиный на этот тип !!!*/

    //Копируем вершины треугольника в буфер
    /*4-ый параметр - то как ГПУ будет управляется с этими данными, куда их запихнет.
    Параметр зависит от частоты установки и частоты использования этих данных. Смотри docs.gl*/
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    //Vertex atrribute берут данные из VBO, который сейчас привязан к GL_ARRAY_BUFFER
    //первый параметр - как раз location = 0 в vertex shader! Второй - размер параметра(типа vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,3 * sizeof(float), (void*)0);
    /*тк сейчас привязан VAO и был передан 0 в первом параметре(и указан в шейдере)
    glVertexAttribPointer, vertex atrribute 0 теперь ассоциируются с position vertex attribute этого VAO.
    Может быть много VertexAttributePointer и походу? можно переключаться между ними*/
    glEnableVertexAttribArray(0);
    //анбиндим VAO - теперь он будет нужен только в render loop, тогда и забиндим
    glBindVertexArray(0);

    //второй треугольник
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    
    glGenBuffers(1, &VBO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    //EBO for a textured rec
    unsigned int EBO2;
    glGenBuffers(1, &EBO2);
    //OpenGL благодаря тэгам бафферов поймет что есть что в VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);


    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3*sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture coords attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);  
    glBindVertexArray(0);

    //-----------------
    //-----Data for a rectangle------
    //сначала все аналогично треугольнику
    unsigned int VAO_rec;
    glGenVertexArrays(1, &VAO_rec);
    glBindVertexArray(VAO_rec);

    unsigned int VBO_rec;
    glGenBuffers(1,&VBO_rec);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_rec);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_rec), vertices_rec, GL_STATIC_DRAW);

    //EBO, его тоже можно запихать в текущй VAO(новый, VAO_rec). Анбиндить его во время настройки VAO нельзя, все слетит.
    unsigned int EBO_rec;
    glGenBuffers(1, &EBO_rec);
    //OpenGL благодаря тэгам бафферов поймет что есть что в VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_rec);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    //ну и аналогичный процесс копирования вершин в буфер
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //анбиндим VAO
    glBindVertexArray(0);
    //------------------
    //устанавливаем размер и положение Viewport-a OpenGL в окне
    glViewport(0,0, 800, 600);
    

    //регистрация callback function, вызывающаяся когда изменяют размер окна. Эти функции могут вызываться при очень разных обстоятельствах
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //setting texture unforms
    ourShader.use();
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    ourShader.setInt("texture2", 1);
    //------render loop-------
    while(!glfwWindowShouldClose(window))
    {
        //input
        processInput(window);

        //render commands go here!
        //выбор цвета для очистки экрана (state-setting func)
        glClearColor(0.4f, 0.3f, 0.3f, 1.0f);
        //установка бита колор буфера, очистка экрана заданным цветом(state-using func)
        glClear(GL_COLOR_BUFFER_BIT);


        //Выбираем способ отрисовки примитивов. Первый прм - приминяем к передней и задней части примитива. Второй - тип отрисовки(п. - GL_LINE/GL_FILL)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);




        //обновляем юниформы
        float offset = 0.0;
        ourShader.setFloat("offset", offset);
        //теперь рендерим треугольник
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        //Прямоугольник с EBO
        //glBindVertexArray(VAO_rec);
        //DrawElements берет индексы из текущего забинденого EBO. Собственно поэтому для облегчения их надо привязывать к VAO(как и все остальное! Core обязывает использовать VAO)
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        //анбиндим VAO
        glBindVertexArray(0);
        //свапает бэк и фронт баферы по сути
        glfwSwapBuffers(window);
        //проверяет сработало ли какое то событие, обновляет состояние окна и вызывает соотв. callback funtions
        glfwPollEvents();
    }
    //---------------

    //удалить окно и все что с ним связано
    glfwTerminate();
    return 0;
}