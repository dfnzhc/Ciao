#include "pch.h"
#include "OpenAssetImportMesh.h"

OpenAssetImportMesh::OpenAssetImportMesh()
{
}

OpenAssetImportMesh::~OpenAssetImportMesh()
{
    Clear();
}

bool OpenAssetImportMesh::Load(const std::string& Filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();

    bool Ret = false;
    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
    if (pScene) {
        Ret = InitFromScene(pScene, Filename);
    }
    else {
        char message[1024];
        sprintf_s(message, "Error loading mesh model\n%s\n", Importer.GetErrorString());
    }

    return Ret;
}

bool OpenAssetImportMesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    glGenVertexArrays(1, &m_vao); 
    glBindVertexArray(m_vao);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0 ; i < m_Entries.size() ; ++i) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
    }

    return InitMaterials(pScene, Filename);
}

void OpenAssetImportMesh::InitMesh(unsigned Index, const aiMesh* paiMesh)
{
    m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;
    
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; ++i) {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
                 glm::vec2(pTexCoord->x, 1.0f-pTexCoord->y),
                 glm::vec3(pNormal->x, pNormal->y, pNormal->z));

        Vertices.push_back(v);
    }

    for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; ++i) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }

    m_Entries[Index].Init(Vertices, Indices);
}

bool OpenAssetImportMesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = Filename.find_last_of("\\");
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "\\";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    for (unsigned int i = 0 ; i < pScene->mNumMaterials ; ++i) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_Textures[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string FullPath = Dir + "\\" + Path.data;
                m_Textures[i] = new Texture();
                if (!m_Textures[i]->Load(FullPath, true)) {
                    char message[1024];
                    sprintf_s(message, "Error loading mesh texture\n%s\n", FullPath.c_str());
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    Ret = false;
                }
                else {
                    printf("Loaded texture '%s'\n", FullPath.c_str());
                }
            }
        }

        // Load a single colour texture matching the diffuse colour if no texture added
        if (!m_Textures[i]) {
		    
            aiColor3D color (1.f,0.f,1.f);
            //pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE,color);

            m_Textures[i] = new Texture();
            BYTE data[3];
            data[0] = (BYTE) (color[2]*255);
            data[1] = (BYTE) (color[1]*255);
            data[2] = (BYTE) (color[0]*255);
            m_Textures[i]->CreateFromData(data, 1, 1, 24, GL_BGR, false);

        }
    }
    return Ret;
}

void OpenAssetImportMesh::Draw()
{
    glBindVertexArray(m_vao);

    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].vbo);
        // 顶点位置
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        // 纹理坐标
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        // 法线
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].ibo);

        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
            m_Textures[MaterialIndex]->Bind(0);
        }

        glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }
}


void OpenAssetImportMesh::Clear()
{
    for (unsigned int i = 0 ; i < m_Textures.size() ; ++i) {
        SAFE_DELETE(m_Textures[i]);
    }
    glDeleteVertexArrays(1, &m_vao);
}

OpenAssetImportMesh::MeshEntry::MeshEntry()
{
    vbo = INVALID_OGL_VALUE;
    ibo = INVALID_OGL_VALUE;
    NumIndices  = 0;
    MaterialIndex = INVALID_MATERIAL;
}

OpenAssetImportMesh::MeshEntry::~MeshEntry()
{
    if (vbo != INVALID_OGL_VALUE)
        glDeleteBuffers(1, &vbo);

    if (ibo != INVALID_OGL_VALUE)
        glDeleteBuffers(1, &ibo);
}

void OpenAssetImportMesh::MeshEntry::Init(const std::vector<Vertex>& Vertices, const std::vector<unsigned>& Indices)
{
    NumIndices = Indices.size();
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * NumIndices, &Indices[0], GL_STATIC_DRAW);
}
