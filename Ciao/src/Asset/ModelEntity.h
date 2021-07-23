#pragma once
#include "Object.h"

namespace Ciao
{
    class OpenAssetImportMesh;
    class ShaderProgram;
    
    class ModelEntity
    {
    public:
        ModelEntity();
        ~ModelEntity() ;

        void Init(std::shared_ptr<OpenAssetImportMesh> mesh);
        void AddShader(std::shared_ptr<ShaderProgram> shader);
        
        void Draw();
        
        void SetDrawShaderIdx(unsigned int idx);
    private:
        std::shared_ptr<OpenAssetImportMesh> m_Mesh;
        std::vector<std::shared_ptr<ShaderProgram>>  m_Shaders;

        unsigned int m_ShaderIdx;
    };
}


