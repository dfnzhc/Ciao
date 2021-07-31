#pragma once

namespace Ciao
{
    struct GLLight
    {
        glm::vec4 Position;
        glm::vec4 Direction;

        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;

        float cutOff;
        float theta;

        GLLight() = default;

        GLLight(const glm::vec4& p, const glm::vec4& dir, const glm::vec4& a, const glm::vec4& diff, const glm::vec4& s,
            float cutOff_, float theta_) : Position(p), Direction(dir), ambient(a), diffuse(diff), specular(s)
        {
            cutOff = cos(glm::radians(cutOff_));
            theta = cos(glm::radians(theta_));
        }

        GLLight(const GLLight& light)
        {
            Position = light.Position;
            Direction = light.Direction;
            ambient = light.ambient;
            diffuse = light.diffuse;
            specular = light.specular;

            cutOff = cos(glm::radians(light.cutOff));
            theta = cos(glm::radians(light.theta));
        }
    };

    const GLsizeiptr LightBufferSize = sizeof(GLLight);
}


