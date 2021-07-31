#pragma once
#include "Object.h"

#include "Renderer/VertexBuffer.h"

namespace Ciao
{
    class Sphere : public Object
    {
    public:
        Sphere();
        ~Sphere();

        void Init(int slicesIn, int stacksIn);
        void Create() override;
        void Draw(std::shared_ptr<ShaderProgram> shader) override;
        void Release() override;
    
    private:
        GLuint m_VAO;
        VertexBufferObjectIndexed m_VBO;
        int m_NumTriangles, m_SlicesIn, m_StacksIn;
    };
}


