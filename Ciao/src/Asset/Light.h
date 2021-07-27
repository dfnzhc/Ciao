#pragma once

namespace Ciao
{
    struct GLLight
    {
        glm::vec3 Position;
        glm::vec3 Direction;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        float cutOff;
        float theta;
    };

    const GLsizeiptr LightBufferSize = sizeof(GLLight);
}


