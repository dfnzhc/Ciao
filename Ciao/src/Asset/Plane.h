#pragma once
#include "Asset/Object.h"
#include "Asset/Texture.h"
#include "Renderer/VertexBuffer.h"

namespace Ciao
{
    class Plane : public Object
    {
    public:
        Plane();
        ~Plane() override;
    
        void Init(std::string textureDirectory, float w, float h, float texRepeat);   
        void Create() override;
        void Draw() override;
        void Release() override;
    private:
        GLuint m_VAO;
        VertexBufferObject m_VBO;
        Texture m_Texture;

        float m_Width, m_Height, m_TexRepeat;
    };
}


