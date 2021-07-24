#pragma once
#include "Asset/CubeMap.h"
#include "Asset/Object.h"
#include "Renderer/VertexBuffer.h"

namespace Ciao
{
    class CubeMap;
    
    class Skybox : public Object
    {
    public:
        Skybox();
        ~Skybox() override;

        void Init(const std::string& texDir, const std::string& suffix = ".jpg");
        void Create() override;
        void Draw() override;
        void Release() override;

        void SetTextureUnit(int texUnit = 0);
        int GetTextureUnit() const { return m_TexUnit; }
        

    private:
        static const std::vector<std::string> Faces;
        
        GLuint m_VAO;
        VertexBufferObject m_VBO;
        CubeMap m_CubeMap;

        int m_TexUnit;
    };
}


