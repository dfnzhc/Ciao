#pragma once
#include "Texture.h"
#include "VertexBufferObject.h"

class Cube
{
public:
    Cube();
    ~Cube();
    void Create(std::string textureDirectory);
    void Draw();
    void Release();

private:
    GLuint m_VAO;
    VertexBufferObjectIndexed m_VBO;
    Texture m_Texture;

    unsigned int m_NumTriangles;
};