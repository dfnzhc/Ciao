#include "pch.h"
#include "Camera.h"
#include "Mouse.h"

namespace Ciao
{
    void CameraPositioner_Oribit::update()
    {
        processMovement();
        processScroll();

        right_ = normalize(cross(glm::vec3{ 0, 1, 0 }, viewDir_));
        up_ = normalize(cross(viewDir_, right_));

        viewMat_ = glm::lookAt(position_, target_, up_);
    }


    void CameraPositioner_Oribit::processMovement()
    {

        glm::vec2 delta{ Mouse::DX() * mouseSpeed_, Mouse::DY() * mouseSpeed_ };

        if (Mouse::ButtonStay(CIAO_INPUT_MOUSE_RIGHT))
        {
            delta.x *= -1.0f;
            delta *= 0.3f;

            glm::vec3 offset = delta.x * right_ + delta.y * up_;

            target_ += offset;
            position_ += offset;
        }

        else if (Mouse::ButtonStay(CIAO_INPUT_MOUSE_LEFT))
        {
            pitch_ += delta.y;
            yaw_ += delta.x;

            if (pitch_ > 89.0f)
                pitch_ = 89.0f;
            if (pitch_ < -89.0f)
                pitch_ = -89.0f;

            position_.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
            position_.y = sin(glm::radians(pitch_));
            position_.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));

            position_ *= distance_;
            position_ += target_;
        }

        viewDir_ = normalize(position_ - target_);
    }

    void CameraPositioner_Oribit::processScroll()
    {
        position_ = target_ + viewDir_ * distance_;
    }
}
