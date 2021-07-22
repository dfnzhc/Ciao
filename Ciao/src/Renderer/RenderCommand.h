#pragma once

#include "Shader.h"

#include <memory>

namespace Ciao
{
    class RenderCommand
    {
    public:
        virtual ~RenderCommand() = default;

        virtual void Execute() = 0;
    };

    class DrawVao : public RenderCommand 
    {
    public:
        DrawVao(GLuint vao, GLuint type, GLuint count, std::shared_ptr<ShaderProgram> shader)
            : m_VAO(vao), m_primType(type), m_primSize(count), m_Shader(shader) {}
        
        void Execute() override;

    private:
        GLuint m_VAO;
        GLuint m_primType;
        GLuint m_primSize;
        std::shared_ptr<ShaderProgram> m_Shader;
    };

    
    class Object;
    class DrawObject : public RenderCommand
    {
    public:
        DrawObject(std::shared_ptr<Object> obj, std::shared_ptr<ShaderProgram> shader)
            : m_Obj(obj), m_Shader(shader) {}
        
        void Execute() override;
    
    private:
        std::shared_ptr<Object> m_Obj;
        std::shared_ptr<ShaderProgram> m_Shader;
    };

    class Framebuffer;
    class PushFramebuffer : public RenderCommand
    {
    public:
        PushFramebuffer(std::shared_ptr<Framebuffer> framebuffer)
            : m_Framebuffer(framebuffer) {}
        
        void Execute() override;
    
    private:
        std::shared_ptr<Framebuffer> m_Framebuffer;
    };

    class PopFramebuffer : public RenderCommand
    {
    public:
        PopFramebuffer(){}
        
        void Execute() override;
    
    private:
        
    };

    class FlushCommandQueue : public RenderCommand
    {
    public:
        FlushCommandQueue(){}
        
        void Execute() override;
    
    private:
        
    };
}
