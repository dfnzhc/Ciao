#include "pch.h"
#include "Camera.h"

#include "Mouse.h"

using glm::normalize;
using glm::cross;

namespace Ciao
{




	CameraPositioner_Oribit::CameraPositioner_Oribit(const vec3& pos, const vec3& target, const vec3& up)
		: position_(pos), orientation_(glm::lookAt(pos, target, up)), up_(up)
	{
	}

	void CameraPositioner_Oribit::update()
	{
	}
}
