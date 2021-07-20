#pragma once
/*
    Copyright 2011 Etay Meiri

    // Modified by Dr Greg Slabaugh to work with OpenGL template

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    class Mesh;
    class Texture;
    
    class OpenAssetImportMesh : public Object
    {
    public:
        OpenAssetImportMesh();
        ~OpenAssetImportMesh();
        bool Load(const std::string& Filepath);
        void Create() override;
        void Draw();
        void Release() override;

        void SetShader(Ref<ShaderProgram> shader);
        
    
    private:
        void processNode(aiNode *node, const aiScene *scene);
        Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture*> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    
        std::vector<Mesh*> m_Meshes;
        std::vector<Texture*> m_Textures;
        Ref<ShaderProgram> m_Shader;
        std::string m_Directory;
        std::set<std::string> m_LoadedTexs;
        GLuint m_vao;
    };

}

