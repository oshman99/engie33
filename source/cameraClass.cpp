#include <cameraClass.h>

//default: glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch): Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp  = up;
    Yaw      = yaw;
    Pitch    = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch): Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp  = glm::vec3(upX, upY, upZ);
    Yaw      = yaw;
    Pitch    = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetVewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
    //custom lookAt implementation
/*     glm::vec3 target = Position + Front;
    glm::vec3 direction = glm::normalize(target - Position);
    glm::vec3 rightAxis = glm::normalize(glm::cross(Up, direction));
    glm::vec3 upAxis = glm::normalize(glm::cross(direction, rightAxis));
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::mat4 translation = glm::mat4(1.0f);
    rotation [0][0] = rightAxis.x;
    rotation [1][0] = rightAxis.y;
    rotation [2][0] = rightAxis.z;
    rotation [0][1] = upAxis.x;
    rotation [1][1] = upAxis.y;
    rotation [2][1] = upAxis.z;
    rotation [0][2] = direction.x;
    rotation [1][2] = direction.y;
    rotation [2][2] = direction.z;
    translation [3][0] = -Position.x;
    translation [3][1] = -Position.y;
    translation [3][2] = -Position.z;
    return rotation * translation; */
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    switch(direction)
    {
    case FORWARD:
        Position += Front * velocity;
        break;
    case BACKWARD:
        Position -= Front * velocity;
        break;
    case LEFT:
        Position -= Right * velocity;
        break;
    case RIGHT:
        Position += Right * velocity;
        break;
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    //make sure that when pitch is out of bound, screen doesn't get flipped
    if(constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }
    
    //update Front, Right and Up vectors using fresh Euler angels
    updateCameraVectors();

}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    //calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw) * cos(glm::radians(Pitch)));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    //also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));//normalization needed because their length gets closer to 0 the more you look up or down => slower movement
    Up    = glm::normalize(glm::cross(Right, Front));
}