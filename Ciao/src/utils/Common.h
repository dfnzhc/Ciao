#pragma once

namespace Ciao
{
    constexpr float Pi				= 3.14159265358979323846f;
    constexpr float Inv_Pi			= 0.31830988618379067154f;
    constexpr float Inv_TwoPi		= 0.15915494309189533577f;
    constexpr float Inv_FourPi		= 0.07957747154594766788f;
    constexpr float Sqrt_Two		= 1.41421356237309504880f;
    constexpr float Inv_Sqrt_Two	= 0.70710678118654752440f;

    constexpr float Epsilon			= 1e-3f;

    #define _USE_MATH_DEFINES
    #include <cmath>
    #include <glm/glm.hpp>
    #include <glm/ext.hpp>

    using glm::mat4;
    using glm::mat3;
    using glm::vec2;
    using glm::vec3;
    using glm::vec4;

    constexpr std::string ResourceDir(const std::string& s) { return std::string("Assets/").append(s); }
    #define Res(s) ResourceDir(s).c_str()
}
