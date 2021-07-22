#pragma once
#include "Renderer/VertexBuffer.h"

namespace Ciao
{
    class CubeMap
    {
    public:
        CubeMap();
        ~CubeMap();
        void Create(const std::vector<std::string>& filenames);

        void Bind(int texUnit = 0);
        void Release();
        
    private:
        bool LoadTexture(const std::string& filename, BYTE** bmpBytes, int& width, int& height);
        
        GLuint m_Texture;
        GLuint m_Sampler;
        
    };
}


