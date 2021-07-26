#include "pch.h"
#include "ModelEntity.h"

#include "OpenAssetImportMesh.h"

namespace Ciao
{
    ModelEntity::ModelEntity() : m_ShaderIdx(0)
    {
        
    }

    ModelEntity::~ModelEntity()
    {
        
    }

    void ModelEntity::Init(std::shared_ptr<OpenAssetImportMesh> mesh)
    {
        m_Mesh = mesh;
    }

    void ModelEntity::AddShader(std::shared_ptr<ShaderProgram> shader)
    {
        m_Shaders.push_back(shader);
    }

    void ModelEntity::Draw()
    {
        m_Shaders[m_ShaderIdx]->UseProgram();
        m_Mesh->Draw();
    }

    void ModelEntity::SetDrawShaderIdx(unsigned idx)
    {
        m_ShaderIdx = idx;
    }
}
