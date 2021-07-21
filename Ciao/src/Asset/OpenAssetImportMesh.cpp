#include "pch.h"
#include "OpenAssetImportMesh.h"

#include "Mesh.h"
#include "Texture.h"

namespace Ciao
{
    OpenAssetImportMesh::OpenAssetImportMesh()
    {
    }

    OpenAssetImportMesh::~OpenAssetImportMesh()
    {
        for (unsigned int i = 0; i < m_Meshes.size(); ++i) {
            delete m_Meshes[i];

            m_Meshes[i] = nullptr;
        }
    }

    bool OpenAssetImportMesh::Load(const std::string& Filepath)
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

    void OpenAssetImportMesh::Create()
    {
    }

    void OpenAssetImportMesh::Draw()
    {
        for(int i = 0; i < m_Textures.size(); ++i)
        {
            //glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            m_Textures[i]->Bind(i);
            m_Shader->SetUniform(m_Textures[i]->GetTextureName(), i);
        }
        
        for (unsigned int i = 0; i < m_Meshes.size(); ++i) {
            m_Meshes[i]->Draw();
        }
    }

    void OpenAssetImportMesh::Release()
    {
    }

    void OpenAssetImportMesh::SetShader(Ref<ShaderProgram> shader)
    {
        m_Shader = shader;
    }

    void OpenAssetImportMesh::processNode(aiNode* node, const aiScene* scene)
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

    Mesh* OpenAssetImportMesh::processMesh(aiMesh* mesh, const aiScene* scene)
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
                vec.y = 1.0f - mesh->mTextureCoords[0][i].y;
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
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN
            /** The texture is combined with the result of the diffuse
     *  lighting equation.
     */
        
        // 1. diffuse maps
        auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. ambient maps
        auto ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ambient");
        textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
        // 4. emissive maps
        auto emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive");
        textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
        // 5 height maps
        auto heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        // 6 normal maps
        auto normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 7 shininess maps
        auto shininessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_shininess");
        textures.insert(textures.end(), shininessMaps.begin(), shininessMaps.end());
        // 8 opacity maps
        auto opacityMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "texture_opacity");
        textures.insert(textures.end(), opacityMaps.begin(), opacityMaps.end());
        // 9 displacement maps
        auto displacementMaps = loadMaterialTextures(material, aiTextureType_DISPLACEMENT, "texture_displacement");
        textures.insert(textures.end(), displacementMaps.begin(), displacementMaps.end());
        // 10 lightMap maps
        auto lightMapMaps = loadMaterialTextures(material, aiTextureType_LIGHTMAP, "texture_lightMap");
        textures.insert(textures.end(), lightMapMaps.begin(), lightMapMaps.end());
        // 11 reflection maps
        auto reflectionMaps = loadMaterialTextures(material, aiTextureType_REFLECTION, "texture_reflection");
        textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());
        // 12 baseColor maps
        auto baseColorMaps = loadMaterialTextures(material, aiTextureType_BASE_COLOR, "texture_baseColor");
        textures.insert(textures.end(), baseColorMaps.begin(), baseColorMaps.end());
        // 13 normalCamera maps
        auto normalCameraMaps = loadMaterialTextures(material, aiTextureType_NORMAL_CAMERA, "texture_normalCamera");
        textures.insert(textures.end(), normalCameraMaps.begin(), normalCameraMaps.end());
        // 14 emissionColor maps
        auto emissionColorMaps = loadMaterialTextures(material, aiTextureType_EMISSION_COLOR, "texture_emissionColor");
        textures.insert(textures.end(), emissionColorMaps.begin(), emissionColorMaps.end());
        // 15 metalness maps
        auto metalnessMaps = loadMaterialTextures(material, aiTextureType_METALNESS, "texture_metalness");
        textures.insert(textures.end(), metalnessMaps.begin(), metalnessMaps.end());
        // 16 diffuseRoughness maps
        auto diffuseRoughnessMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_diffuseRoughness");
        textures.insert(textures.end(), diffuseRoughnessMaps.begin(), diffuseRoughnessMaps.end());
        // 17 AO maps
        auto AOMaps = loadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, "texture_AO");
        textures.insert(textures.end(), AOMaps.begin(), AOMaps.end());
        // 17 unknown maps
        auto unknownMaps = loadMaterialTextures(material, aiTextureType_UNKNOWN, "texture_unknown");
        textures.insert(textures.end(), unknownMaps.begin(), unknownMaps.end());
        // aiTextureType_SHININESS = 7,
        // aiTextureType_OPACITY = 8,
        // aiTextureType_DISPLACEMENT = 9,
        // aiTextureType_LIGHTMAP = 10,
        // aiTextureType_REFLECTION = 11,
        // aiTextureType_BASE_COLOR = 12,
        // aiTextureType_NORMAL_CAMERA = 13,
        // aiTextureType_EMISSION_COLOR = 14,
        // aiTextureType_METALNESS = 15,
        // aiTextureType_DIFFUSE_ROUGHNESS = 16,
        // aiTextureType_AMBIENT_OCCLUSION = 17,
        
        m_Textures = std::move(textures);
        // return a mesh object created from the extracted mesh data
        auto resMesh = new Mesh();
        resMesh->Init(
            std::forward<std::vector<Vertex>>(vertices),
            std::forward<std::vector<UINT>>(indices));

        return resMesh;
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture*> OpenAssetImportMesh::loadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                                                    std::string typeName)
    {
        std::vector<Texture*> textures;
        for (unsigned int i = 0, j = 1; i < mat->GetTextureCount(type); ++i) {
            aiString str;
            if (mat->GetTexture(type, i, &str) == AI_SUCCESS) {
                if (m_LoadedTexs.count(str.C_Str()) > 0) {
                    continue;
                }
                
                std::string FullPath = m_Directory + "\\" + str.data;
                auto tex = new Texture();

                if (!tex->Load(FullPath, true)) {
                    aiColor3D color(1.f, 0.f, 1.f);
                    //pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE,color);

                    /// 载入纹理失败，创建一个品红色纹理
                    BYTE data[3];
                    data[0] = (BYTE)(color[2] * 255);
                    data[1] = (BYTE)(color[1] * 255);
                    data[2] = (BYTE)(color[0] * 255);
                    tex->CreateFromData(data, 1, 1, 24, GL_BGR, false);
                }
                std::string texName = typeName + std::to_string(j++);
                
                tex->SetTextureName(texName);
                
                textures.push_back(tex);
                m_LoadedTexs.insert(str.C_Str());
            }
        }

        return textures;
    }
}
