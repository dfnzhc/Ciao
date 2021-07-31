#pragma once
#include "Asset/Object.h"
#include "Renderer/VertexBuffer.h"

namespace Ciao
{
    class Plane : public Object
    {
    public:
        Plane();
        ~Plane() override;
    
        void Init(float w, float h, float texRepeat);   
        void Create() override;
        void Draw(std::shared_ptr<ShaderProgram> shader) override;
        void Release() override;
    private:
        GLuint m_VAO;
        VertexBufferObject m_VBO;

        float m_Width, m_Height, m_TexRepeat;
    };
}


