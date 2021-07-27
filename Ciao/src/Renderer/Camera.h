#pragma once

#include "glm/gtc/matrix_transform.hpp"

namespace Ciao
{
    class Camera
    {
    public:
        Camera(
            glm::vec3 position = glm::vec3(0.0f, 5.0f, 5.0f * sqrtf(3)),
            glm::vec3 viewPoint = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

        void Init();

        void Update();

        glm::mat4 GetViewMatrix();

        void SetProjectionMatrix(float fov, float aspectRatio, float nearClippingPlane, float farClippingPlane);
        glm::mat4 GetProjectionMatrix();

        void ProcessMouseMovement();
        void ProcessMouseScroll();

        static void UpdateDistance(float delta) { m_Distance += delta; }

        glm::vec3 GetPosition() { return m_Position; }

    private:
        void UpdateCameraVectors();

    private:
        const glm::vec3 WorldUp{0.0f, 1.0f, 0.0f};
        constexpr static float MouseSensitivity = 0.1f;
        constexpr static float ZoomLength = 1.0f;
        
        glm::vec3 m_Position;
        glm::vec3 m_ViewDir;
        glm::vec3 m_UpDir;
        glm::vec3 m_RightDir;

        glm::vec3 m_ViewPoint;
        static float m_Distance;

        glm::vec2 m_HalfWindowSize;

        glm::mat4 m_ProjectionMat;
        glm::mat4 m_ViewMat;

        // euler Angles
        float m_Yaw;
        float m_Pitch;
    };
}


