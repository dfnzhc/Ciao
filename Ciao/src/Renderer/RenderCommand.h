#pragma once

#include "Shader.h"

#include <memory>

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
        VAORenderCommand(GLuint vao, GLuint type, GLuint count, std::shared_ptr<ShaderProgram> shader)
            : m_VAO(vao), m_primType(type), m_primSize(count), m_Shader(shader) {}
        
        void Draw() override;

    private:
        GLuint m_VAO;
        GLuint m_primType;
        GLuint m_primSize;
        std::shared_ptr<ShaderProgram> m_Shader;
    };

    
    class Object;
    class ObjRenderCommand : public RenderCommand
    {
    public:
        ObjRenderCommand(std::shared_ptr<Object> obj, std::shared_ptr<ShaderProgram> shader)
            : m_Obj(obj), m_Shader(shader) {}
        
        void Draw() override;
    
    private:
        std::shared_ptr<Object> m_Obj;
        std::shared_ptr<ShaderProgram> m_Shader;
    };

    
}
