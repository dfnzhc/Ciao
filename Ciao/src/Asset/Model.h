#pragma once

#include <map>
#include <set>
#include <vector>
#include <assimp/Importer.hpp>         // C++ importer interface
#include <assimp/scene.h>              // Output data structure
#include <assimp/PostProcess.h>        // Post processing flags

#include "Asset/Object.h"
#include "Renderer/Shader.h"

#define INVALID_OGL_VALUE 0xFFFFFFFF
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

namespace Ciao
{
    class MeshObj;
    
    class Model : public Object
    {
    public:
        Model();
        ~Model();
        bool Load(const std::string& Filepath);
        void Create() override;
        void Draw(std::shared_ptr<ShaderProgram> shader) override;
        void Release() override;
    
    private:
        void processNode(aiNode *node, const aiScene *scene);
        MeshObj* processMesh(aiMesh *mesh, const aiScene *scene);
    
        std::vector<MeshObj*> m_Meshes;
        std::string m_Directory;
        GLuint m_vao;
    };

}

