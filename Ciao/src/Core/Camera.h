#pragma once

namespace Ciao
{
	enum Camera_Mode
	{
		None = 0, Orbit, FirstPerson
	};

	class CameraPositioner
	{
	public:
		virtual ~CameraPositioner() = default;
		virtual mat4 getViewMatrix() const = 0;
		virtual vec3 getPosition() const = 0;
	};

	class Camera final
	{
	public:
		explicit Camera(CameraPositioner& positioner);

		Camera(const Camera&) = default;
		Camera& operator=(const Camera&) = default;

		mat4 getViewMatrix() const { positioner_->getViewMatrix(); }
		mat4 getPosition() const { positioner_->getPosition(); }

	private:
		const CameraPositioner* positioner_;
	};


	class CameraPositioner_Oribit final : public CameraPositioner
	{
	public:
		CameraPositioner_Oribit() = default;

		CameraPositioner_Oribit(const vec3& pos, const vec3& target, const vec3& up);

		void update();

		mat4 getViewMatrix() const override { return viewMat_; }
		vec3 getPosition() const override { return position_; }


	private:
		vec3 position_{0.0f, 10.0f, 10.0f};
		glm::quat orientation_{ vec3{0.0f} };
		vec3 up_{ 0.0f, 0.0f, 1.0f };


		vec3 right_;


		mat4 viewMat_{1.0f};
	};
}
