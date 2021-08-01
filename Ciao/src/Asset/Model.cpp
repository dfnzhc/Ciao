#include "pch.h"
#include "Model.h"

#include "Mesh.h"
#include "Texture.h"

namespace Ciao
{
    Model::Model()
    {
    }

    Model::~Model()
    {
        Release();
    }

    bool Model::Load(const std::string& Filepath)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            Filepath,
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            CIAO_CORE_ERROR("ERROR::ASSIMP:: {}", importer.GetErrorString());
            return false;
        }

        // retrieve the directory path of the filepath// Extract the directory part from the file name
        std::string::size_type SlashIndex = Filepath.find_last_of("\\");

        if (SlashIndex == std::string::npos) {
            m_Directory = ".";
        }
        else if (SlashIndex == 0) {
            m_Directory = "\\";
        }
        else {
            m_Directory = Filepath.substr(0, SlashIndex);
        }

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
        return true;
    }

    void Model::Create()
    {
    }

    void Model::Draw(std::shared_ptr<ShaderProgram> shader)
    {
        for (unsigned int i = 0; i < m_Meshes.size(); ++i) {
            m_Meshes[i]->Draw(shader);
        }
    }

    void Model::Release()
    {
        for (unsigned int i = 0; i < m_Meshes.size(); ++i) {
            delete m_Meshes[i];

            m_Meshes[i] = nullptr;
        }
    }

    void Model::processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_Meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<UINT> indices;
        std::vector<Texture*> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            Vertex vertex;
            glm::vec3 vector;
            // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->HasTextureCoords(0)) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        
        // return a mesh object created from the extracted mesh data
        auto resMesh = new Mesh();
        resMesh->Init(
            std::forward<std::vector<Vertex>>(vertices),
            std::forward<std::vector<UINT>>(indices));

        return resMesh;
    }
}
