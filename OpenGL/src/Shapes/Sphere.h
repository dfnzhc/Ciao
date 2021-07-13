#pragma once
#include "Texture.h"
#include "VertexBufferObject.h"

class Sphere
{
public:
    Sphere();
    ~Sphere();
    void Create(std::string textureDirectory, int slicesIn, int stacksIn);
    void Draw();
    void Release();

private:
    GLuint m_VAO;
    VertexBufferObjectIndexed m_VBO;
    Texture m_Texture;
    int m_NumTriangles;
};