#pragma once

#include "Object.h"

#include "Renderer/VertexBuffer.h"
#include "Texture.h"

namespace Ciao
{
    class Cube : public Object
    {
    public:
        Cube();
        ~Cube();

        void Init(std::string textureDirectory);
        
        void Create() override;
        void Draw() override;
        void Release() override;
    private:
        GLuint m_VAO;
        VertexBufferObjectIndexed m_VBO;
        Texture m_Texture;

        unsigned int m_NumTriangles;
    };
}


