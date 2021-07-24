#pragma once

namespace Ciao
{
    glm::mat3 ComputeNormalMatrix(const glm::mat4 &modelViewMatrix)
    {
        return glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));
    }
}