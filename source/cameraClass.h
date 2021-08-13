#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <vector>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT
};

//default camera movements
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

//не забывать: дефолтные параметры в .h файле должны быть
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    //constructors with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    //constructor with scalars
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
    //returns the view matrix calculated using Euler Angels and the LookAt matrix
    glm::mat4 GetVewMatrix();
    //process input received from any keyoard-like input system. Accepts input parameter in the form of camera defined Enum(abstraction from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    //process input received from a mouse input system. Expects the offset value in both the x and y direction
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    //process input from a mouse scroll-wheel event. So, input only on y-axis - wheel
    void ProcessMouseScroll(float yoffset);
private:
    //calculates the front vector from Camera's new Euler Angels
    void updateCameraVectors();


};

#endif