#pragma once

namespace Ciao
{
	enum Camera_Mode
	{
		None = 0, Orbit, FirstPerson
	};

	class Camera
	{
	public:
		Camera();

		void update();

		void setOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
		void setPerspective(float fovy, float aspect, float zNear, float zFar);

		mat4 getViewMatrix() const { return mView_; }
		mat4 getProjMatrix() const { return mProj_; }


		void updateDistance(float delta) { if (std::abs(distToViewPoint_ + delta) < Epsilon) return; distToViewPoint_ += delta; }

	private:

		void processMouseMovement();
		void processMouseScroll();

		void updateCameraViewMatrix();


		const vec3 WorldUp{ 0.0f, 1.0f, 0.0f };

		vec3 viewVec_;
		vec3 upVec_;
		vec3 rightVec_;

		vec3 position_{0.0, 5.0f * Sqrt_Two, 5.0f * Sqrt_Two};
		vec3 viewPoint_{0};

		float fovy_ = 45.0f;
		float aspect_;
		float zNear_ = 0.1f;
		float zFar_ = 1000.0f;

		mat4 mProj_;
		mat4 mView_;


		float distToViewPoint_ = 10.0f;

		float yaw_ = -90.0f;
		float pitch_ = -45.0f;
	};
}
