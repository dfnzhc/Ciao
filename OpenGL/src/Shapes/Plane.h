#pragma once
#include "Texture.h"
#include "VertexBufferObject.h"

class Plane
{
public:
    Plane();
    ~Plane();
    void Create(std::string textureDirectory, float fWidth, float fHeight, float fTextureRepeat);
    void Draw();
    void Release();

private:
    GLuint m_VAO;
    VertexBufferObject m_VBO;
    Texture m_Texture;
    float m_Width;
    float m_Height;
};