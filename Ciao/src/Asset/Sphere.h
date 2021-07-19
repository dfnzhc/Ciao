#pragma once
#include "Object.h"

#include "Renderer/GLBuffer.h"
#include "Texture.h"

namespace Ciao
{
    class Sphere : public Object
    {
    public:
        Sphere();
        ~Sphere();

        void Init(std::string textureDirectory, int slicesIn, int stacksIn);
        void Create() override;
        void Draw() override;
        void Release() override;
    
    private:
        GLuint m_VAO;
        VertexBufferObjectIndexed m_VBO;
        Texture m_Texture;
        int m_NumTriangles, m_SlicesIn, m_StacksIn;
    };
}


