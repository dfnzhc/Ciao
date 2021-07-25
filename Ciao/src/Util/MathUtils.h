#pragma once

namespace Ciao
{
    template <typename T>
    T clamp(T v, T a, T b)
    {
        if (v < a) return a;
        if (v > b) return b;
        return v;
    }
    
    glm::mat3 ComputeNormalMatrix(const glm::mat4 &modelViewMatrix)
    {
        return glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));
    }
}