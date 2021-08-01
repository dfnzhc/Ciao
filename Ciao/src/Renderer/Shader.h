#pragma once

#include "glm/glm.hpp"

namespace Ciao
{
    /// OpenGL shader 的封装抽象
    class Shader
    {
    public:
        Shader();

        bool LoadShader(std::string sFile, int iType);
        void DeleteShader();

        bool GetLinesFromFile(std::string sFile, bool bIncludePart, std::vector<std::string>* vResult);

        bool IsLoaded() const;
        glm::uint GetShaderID() const;

    private:
        glm::uint m_shaderID;   /// shader 的id
        int m_ShaderType;       /// shader 的种类：GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
        bool m_isLoaded;        /// shader 是否被载入并编译
    };

    /// OpenGL shader 程序的封装抽象
    class ShaderProgram
    {
    public:
        ShaderProgram();

        void CreateProgram();
        void DeleteProgram();

        bool AddShaderToProgram(const Shader* shShader);
        bool LinkProgram();

        void UseProgram();

        glm::uint GetProgramID();

        // Setting vectors
        void SetUniform(const std::string& sName, glm::vec2* vVectors, int iCount = 1);
        void SetUniform(const std::string& sName, const glm::vec2 vVector);
        void SetUniform(const std::string& sName, glm::vec3* vVectors, int iCount = 1);
        void SetUniform(const std::string& sName, const glm::vec3 vVector);
        void SetUniform(const std::string& sName, glm::vec4* vVectors, int iCount = 1);
        void SetUniform(const std::string& sName, const glm::vec4 vVector);

        // Setting floats
        void SetUniform(const std::string& sName, float* fValues, int iCount = 1);
        void SetUniform(const std::string& sName, const float fValue);

        // Setting 3x3 matrices
        void SetUniform(const std::string& sName, glm::mat3* mMatrices, int iCount = 1);
        void SetUniform(const std::string& sName, const glm::mat3 mMatrix);

        // Setting 4x4 matrices
        void SetUniform(const std::string& sName, glm::mat4* mMatrices, int iCount = 1);
        void SetUniform(const std::string& sName, const glm::mat4 mMatrix);

        // Setting integers
        void SetUniform(const std::string& sName, int* iValues, int iCount = 1);
        void SetUniform(const std::string& sName, const int iValue);


    private:
        glm::uint m_ProgramID; // OpenGL Shader 程序的索引 
        bool m_isLinked; // 是否链接了 Shader
    };


    static void ReadShaderFile(const std::vector<std::string>& shaderFileNames, std::vector<Shader>& shShaders)
    {
        for (int i = 0; i < (int)shaderFileNames.size(); i++) {
            std::string sExt = shaderFileNames[i].substr((int)shaderFileNames[i].size() - 4, 4);
            int iShaderType;
            if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
            else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
            else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
            else if (sExt == "tesc") iShaderType = GL_TESS_CONTROL_SHADER;
            else if (sExt == "tese") iShaderType = GL_TESS_EVALUATION_SHADER;
            else iShaderType = GL_COMPUTE_SHADER;
            Shader shader;

            shader.LoadShader(GetAssetDir() + "Shaders\\" + shaderFileNames[i], iShaderType);
            shShaders.push_back(shader);
        }
    }

    static void AddShaderToPrograme(const std::vector<Shader>& shaders,
                                std::vector<std::shared_ptr<ShaderProgram>>& shaderPrograms,
                                const std::vector<GLuint>& indices)
    {
        auto tShader = std::make_shared<ShaderProgram>();
        tShader->CreateProgram();

        for (auto id : indices) {
            CIAO_ASSERT(id < shaders.size(), "Shader index out of range!");

            tShader->AddShaderToProgram(&shaders[id]);
        }
        tShader->LinkProgram();
        
        shaderPrograms.push_back(tShader);
    }

    class GLBuffer
    {
    public:
        GLBuffer(GLsizeiptr size, const void* data, GLbitfield flags);
        ~GLBuffer();

        GLuint getHandle() const { return m_Handle; }

    private:
        GLuint m_Handle;
    };
}
