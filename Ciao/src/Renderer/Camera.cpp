#include "pch.h"

#include "Camera.h"
#include "Mouse.h"
#include "Application.h"

namespace Ciao
{
    float Camera::m_Distance = 4.0f;
    
    Camera::Camera(glm::vec3 position, glm::vec3 viewPoint, glm::vec3 up)
        : m_Position(position), m_ViewPoint(viewPoint), m_UpDir(up)
    {
        
    }

    void Camera::Init()
    {
        m_ViewDir = m_Position - m_ViewPoint;
        //m_Distance = m_ViewDir.length();
        m_ViewDir = glm::normalize(m_ViewDir);
        
        m_RightDir = glm::normalize(glm::cross(WorldUp, m_ViewDir));
        m_UpDir = glm::normalize(glm::cross(m_ViewDir, m_RightDir));

        m_ViewMat = glm::lookAt(m_Position, m_ViewPoint, m_UpDir);
        
        uint32_t w, h;
        Application::GetAppInst().GetWindowSize(w, h);
        m_HalfWindowSize = glm::vec2(0.5f * w, 0.5f * h);

        m_Yaw = -90.0f;
        m_Pitch = 30.0f;
    }

    void Camera::Update()
    {
        ProcessMouseMovement();
        ProcessMouseScroll();
    }

    glm::mat4 Camera::GetViewMatrix()
    {
        return m_ViewMat;
    }

    void Camera::SetProjectionMatrix(float fov, float aspectRatio, float nearClippingPlane, float farClippingPlane)
    {
        m_ProjectionMat = glm::perspective(glm::radians(fov), aspectRatio, nearClippingPlane, farClippingPlane);
    }

    glm::mat4 Camera::GetProjectionMatrix()
    {
        return m_ProjectionMat;
    }

    void Camera::ProcessMouseMovement()
    {
        if (Mouse::ButtonStay(CIAO_INPUT_MOUSE_LEFT)) {
            float dx = Mouse::DX() * MouseSensitivity, dy = Mouse::DY() * MouseSensitivity;
            m_Yaw += dx;
            m_Pitch += dy;

            if (m_Pitch > 89.0f)
                m_Pitch = 89.0f;
            if (m_Pitch < -89.0f)
                m_Pitch = -89.0f;

            m_Position.x = cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
            m_Position.y = sin(glm::radians(m_Pitch));
            m_Position.z = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));

            m_Position *= m_Distance;

            m_ViewDir = glm::normalize(m_Position - m_ViewPoint);
        }

        UpdateCameraVectors();
    }

    void Camera::ProcessMouseScroll()
    {
        // if (m_Distance > 1 && Mouse::isScorllUp()) {
        //     m_Distance -= ZoomLength;
        //     
        // }
        // else if (Mouse::isScorllDwon()){
        //     m_Distance += ZoomLength;
        // }

        m_Position = m_ViewPoint + m_ViewDir * m_Distance;
        
        UpdateCameraVectors();
    }

    void Camera::UpdateCameraVectors()
    {
        m_RightDir  = glm::normalize(glm::cross(WorldUp, m_ViewDir));
        m_UpDir     = glm::normalize(glm::cross(m_ViewDir, m_RightDir));

        m_ViewMat   = glm::lookAt(m_Position, m_ViewPoint, m_UpDir);
    }
}
