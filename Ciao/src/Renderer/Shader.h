#pragma once

#include "glm/glm.hpp"

namespace Ciao
{
    /// OpenGL shader 的封装抽象
    class Shader
    {
    public:
        Shader();
        Shader(const std::string& filename);

        bool LoadShader(const std::string&, int iType);
        void DeleteShader();

        bool GetLinesFromFile(const std::string&, bool bIncludePart, std::vector<std::string>* vResult);

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
        ShaderProgram(const Shader& a);
        ShaderProgram(const Shader& a, const Shader& b);
        ShaderProgram(const Shader& a, const Shader& b, const Shader& c);
        ShaderProgram(const Shader& a, const Shader& b, const Shader& c, const Shader& d);
        ShaderProgram(const Shader& a, const Shader& b, const Shader& c, const Shader& d, const Shader& e);

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
            shShaders.push_back(Shader(shaderFileNames[i]));
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
        
        char buffer[8192];
        GLsizei length = 0;
        glGetProgramInfoLog(tShader->GetProgramID(), sizeof(buffer), &length, buffer);
        if (length)
        {
            CIAO_CORE_ERROR("{}", buffer);
            assert(false);
        }
        
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
