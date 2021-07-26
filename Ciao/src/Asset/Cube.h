#pragma once

#include "Object.h"

#include "Renderer/VertexBuffer.h"

namespace Ciao
{
    class Cube : public Object
    {
    public:
        Cube();
        ~Cube();

        void Init();
        
        void Create() override;
        void Draw() override;
        void Release() override;
    private:
        GLuint m_VAO;
        VertexBufferObjectIndexed m_VBO;

        unsigned int m_NumTriangles;
    };
}


