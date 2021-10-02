#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>

using glm::mat4;
using glm::mat3;
using glm::vec2;
using glm::vec3;
using glm::vec4;


namespace Ciao
{
	//// Convert radians to degrees
	inline float radToDeg(float value) { return value * (180.0f / Pi); }

	/// Convert degrees to radians
	inline float degToRad(float value) { return value * (Pi / 180.0f); }

}
