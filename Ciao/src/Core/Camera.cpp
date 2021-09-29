#include "pch.h"
#include "Camera.h"

#include "Mouse.h"

using glm::normalize;
using glm::cross;

namespace Ciao
{
	Camera::Camera()
	{
		viewVec_ = normalize(position_ - viewPoint_);
		rightVec_ = normalize(cross(WorldUp, viewVec_));
		upVec_ = normalize(cross(viewVec_, rightVec_));

		mView_ = glm::lookAt(position_, viewPoint_, upVec_);


	}

	void Camera::update()
	{
		processMouseMovement();
		processMouseScroll();

		updateCameraViewMatrix();
	}

	void Camera::setOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		mProj_ = glm::ortho(left, right, bottom, top, zNear, zFar);

		zNear_ = zNear;
		zFar_ = zFar;
	}

	void Camera::setPerspective(float fovy, float aspect, float zNear, float zFar)
	{
		mProj_ = glm::perspective(fovy, aspect, zNear, zFar);

		zNear_ = zNear;
		zFar_ = zFar;
	}

	void Camera::processMouseMovement()
	{
		constexpr float MouseSensitivity = 0.1f;
		constexpr float ZoomLength = 1.0f;

		if (Mouse::ButtonStay(CIAO_INPUT_MOUSE_RIGHT)) {
			float dx = -Mouse::DX() * MouseSensitivity * 0.1f, dy = Mouse::DY() * MouseSensitivity * 0.1f;
			glm::vec3 offset = dx * rightVec_ + dy * upVec_;

			viewPoint_ += offset;
			position_ += offset;
			viewVec_ = normalize(position_ - viewPoint_);
		}

		if (Mouse::ButtonStay(CIAO_INPUT_MOUSE_LEFT)) {
			float dx = Mouse::DX() * MouseSensitivity, dy = Mouse::DY() * MouseSensitivity;
			yaw_ += dx;
			pitch_ += dy;

			if (pitch_ > 89.0f)
				pitch_ = 89.0f;
			if (pitch_ < -89.0f)
				pitch_ = -89.0f;

			position_.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
			position_.y = sin(glm::radians(pitch_));
			position_.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));

			position_ *= distToViewPoint_;
			position_ += viewPoint_;

			viewVec_ = normalize(position_ - viewPoint_);
		}
	}

	void Camera::processMouseScroll()
	{
		position_ = viewPoint_ + viewVec_ * distToViewPoint_;
	}

	void Camera::updateCameraViewMatrix()
	{
		rightVec_ = normalize(cross(WorldUp, viewVec_));
		upVec_ = normalize(cross(viewVec_, rightVec_));

		mView_ = glm::lookAt(position_, viewPoint_, upVec_);
	}
}
