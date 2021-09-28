#pragma once
#include "Renderer/Shader.h"
#include "Utils/Scene/Scene.h"
#include "Utils/Scene/VtxData.h"

namespace Ciao
{
    struct MaterialDescription;
    class Texture;

    struct SceneData
    {
        SceneData(
        const char* meshFile,
        const char* sceneFile,
        const char* materialFile);

        void LoadScene(const char* sceneFile);
        
        std::vector<Texture> allMaterialTextures_;

        MeshFileHeader header_;
        MeshData meshData_;

        Scene scene_;
        std::vector<MaterialDescription> materials_;
        std::vector<DrawData> shapes_;
    };

    class GLScene final
    {
    public:
        explicit GLScene(const SceneData& data);
        ~GLScene();
        void Draw(std::shared_ptr<ShaderProgram> shader);
        

        GLScene(const GLScene&) = delete;
        GLScene(GLScene&&) = default;
        
    private:
        GLuint vao_;
        uint32_t numIndices_;
        GLsizei numCommands_;

        GLBuffer bufferIndices_;
        GLBuffer bufferVertices_;
        GLBuffer bufferMaterials_;

        GLBuffer bufferIndirect_;

        GLBuffer bufferModelMatrices_;
    };
}

