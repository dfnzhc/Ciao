#pragma once
#include <memory>

#include "Shader.h"
#include <glad/glad.h>

namespace Ciao
{
    class RenderCommand
    {
    public:
        virtual ~RenderCommand() = default;

        virtual void Draw() = 0;
    };

    class VAORenderCommand : public RenderCommand
    {
    public:
        VAORenderCommand(GLuint vao, GLuint type, GLuint count, std::weak_ptr<ShaderProgram> shader)
            : m_VAO(vao), m_primType(type), m_primSize(count), m_Shader(shader) {}
        
        void Draw() override;

    private:
        GLuint m_VAO;
        GLuint m_primType;
        GLuint m_primSize;
        std::weak_ptr<ShaderProgram> m_Shader;
    };

    
}
