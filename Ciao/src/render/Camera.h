#pragma once

#include "Mouse.h"
#include "utils/Common.h"

namespace Ciao
{
    class CameraPositioner
    {
    public:
        virtual ~CameraPositioner() = default;
        virtual glm::mat4 getViewMatrix() const = 0;
        virtual glm::vec3 getPosition() const = 0;
    };

    class Camera final
    {
    public:
        explicit Camera(CameraPositioner& positioner) : positioner_(&positioner)
        {
        }

        Camera(const Camera&) = default;
        Camera& operator=(const Camera&) = default;

        glm::mat4 getViewMatrix() const { return positioner_->getViewMatrix(); }
        glm::vec3 getPosition() const { return positioner_->getPosition(); }

    private:
        const CameraPositioner* positioner_;
    };


    class CameraPositioner_Oribit final : public CameraPositioner
    {
    public:
        //CameraPositioner_Oribit() = default;

        CameraPositioner_Oribit(const glm::vec3& pos = glm::vec3(0.0f, 5.0f, 5.0f * sqrtf(3)),
                                const glm::vec3& target = glm::vec3(0)) : position_(pos), target_(target)
        {
            viewDir_ = normalize(pos - target);
            right_ = normalize(cross(glm::vec3{0, 1, 0}, viewDir_));
            up_ = normalize(cross(viewDir_, right_));

            viewMat_ = glm::lookAt(position_, target_, up_);

            distance_ = distance(pos, target);

            pitch_ = 30;
            yaw_ = 90;

            
        }

        void update();

        virtual glm::mat4 getViewMatrix() const override { return viewMat_; }
        virtual glm::vec3 getPosition() const override { return position_; }

        static void zoom(float delta)
        {
            if (std::abs(distance_ + delta) < 1e-3f)
                return;

            distance_ += delta;
        }

    public:
        float mouseSpeed_ = 0.1f;

    private:
        void processMovement();
        void processScroll();

        glm::vec3 position_{0.0f, 10.0f, 10.0f};


        glm::vec3 viewDir_{glm::vec3{0.0f}};
        glm::vec3 up_{0.0f, 0.0f, 1.0f};
        glm::vec3 right_;

        glm::vec3 target_;

        glm::mat4 viewMat_{1.0f};

        inline static float distance_ = 10.0f;

        float pitch_;
        float yaw_;
    };
}
