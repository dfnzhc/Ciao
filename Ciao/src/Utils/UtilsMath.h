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


	struct BoundingBox
	{
		vec3 min_;
		vec3 max_;
		BoundingBox() = default;

		BoundingBox(const vec3& min, const vec3& max) : min_(glm::min(min, max)), max_(glm::max(min, max))
		{
		}

		BoundingBox(const vec3* points, size_t numPoints)
		{
			vec3 vmin(std::numeric_limits<float>::max());
			vec3 vmax(std::numeric_limits<float>::lowest());

			for (size_t i = 0; i != numPoints; i++)
			{
				vmin = glm::min(vmin, points[i]);
				vmax = glm::max(vmax, points[i]);
			}
			min_ = vmin;
			max_ = vmax;
		}

		vec3 getSize() const { return vec3(max_[0] - min_[0], max_[1] - min_[1], max_[2] - min_[2]); }
		vec3 getCenter() const { return 0.5f * vec3(max_[0] + min_[0], max_[1] + min_[1], max_[2] + min_[2]); }

		void transform(const glm::mat4& t)
		{
			vec3 corners[] = {
				vec3(min_.x, min_.y, min_.z),
				vec3(min_.x, max_.y, min_.z),
				vec3(min_.x, min_.y, max_.z),
				vec3(min_.x, max_.y, max_.z),
				vec3(max_.x, min_.y, min_.z),
				vec3(max_.x, max_.y, min_.z),
				vec3(max_.x, min_.y, max_.z),
				vec3(max_.x, max_.y, max_.z),
			};
			for (auto& v : corners)
				v = vec3(t * vec4(v, 1.0f));
			*this = BoundingBox(corners, 8);
		}

		BoundingBox getTransformed(const glm::mat4& t) const
		{
			BoundingBox b = *this;
			b.transform(t);
			return b;
		}

		void combinePoint(const vec3& p)
		{
			min_ = glm::min(min_, p);
			max_ = glm::max(max_, p);
		}
	};

	template <typename T>
	T clamp(T v, T a, T b)
	{
		if (v < a) return a;
		if (v > b) return b;
		return v;
	}
}
