#include "pch.h"
#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : m_FrontVec(glm::vec3(0.0f, 0.0f, -1.0f)),
    MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    m_isFirstMouse = true;
    m_Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : m_FrontVec(glm::vec3(0.0f, 0.0f, -1.0f)),
    MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    m_isFirstMouse = true;
    m_Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return m_ViewMat;
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        m_Position += m_FrontVec * velocity;
    if (direction == BACKWARD)
        m_Position -= m_FrontVec * velocity;
    if (direction == LEFT)
        m_Position -= m_RightVec * velocity;
    if (direction == RIGHT)
        m_Position += m_RightVec * velocity;

    m_ViewMat = glm::lookAt(m_Position, m_Position + m_FrontVec, m_UpVec);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
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

void Camera::SetProjectionMatrix(float aspectRatio, float nearClippingPlane, float farClippingPlane)
{
    m_ProjectionMat = glm::perspective(glm::radians(Zoom), aspectRatio, nearClippingPlane, farClippingPlane);
}

glm::mat4 Camera::GetProjectionMatrix()
{
    return m_ProjectionMat;
}

void Camera::SetPosition(const glm::vec3& pos)
{
    m_Position = pos;
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    m_FrontVec = glm::normalize(front);
    // also re-calculate the Right and Up vector
    m_RightVec = glm::normalize(glm::cross(m_FrontVec, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_UpVec    = glm::normalize(glm::cross(m_RightVec, m_FrontVec));

    m_ViewMat = glm::lookAt(m_Position, m_Position + m_FrontVec, m_UpVec);
}
