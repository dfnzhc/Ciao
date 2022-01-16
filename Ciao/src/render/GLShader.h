#pragma once

#include <glad/glad.h>

namespace Ciao
{
    class GLShader
    {
    public:
        explicit GLShader(const char* fileName);
        GLShader(GLenum type, const char* text, const char* debugFileName = "");
        ~GLShader();

        GLenum getType() const { return type_; }

        GLuint getHandle() const { return handle_; }

    private:
        GLenum type_;
        GLuint handle_;
    };

    GLenum GetShaderTypeFromFileName(const char* fileName);

    class GLProgram
    {
    public:
        GLProgram(const GLShader& a);
        GLProgram(const GLShader& a, const GLShader& b);
        GLProgram(const GLShader& a, const GLShader& b, const GLShader& c);
        GLProgram(const GLShader& a, const GLShader& b, const GLShader& c, const GLShader& d, const GLShader& e);
        ~GLProgram();

        void useProgram() const;
        GLuint getHandle() const { return handle_; }

        // Setting vectors
        void SetUniform(std::string sName, glm::vec2* vVectors, int iCount = 1);
        void SetUniform(std::string sName, const glm::vec2 vVector);
        void SetUniform(std::string sName, glm::vec3* vVectors, int iCount = 1);
        void SetUniform(std::string sName, const glm::vec3 vVector);
        void SetUniform(std::string sName, glm::vec4* vVectors, int iCount = 1);
        void SetUniform(std::string sName, const glm::vec4 vVector);

        // Setting floats
        void SetUniform(std::string sName, float* fValues, int iCount = 1);
        void SetUniform(std::string sName, const float fValue);

        // Setting 3x3 matrices
        void SetUniform(std::string sName, glm::mat3* mMatrices, int iCount = 1);
        void SetUniform(std::string sName, const glm::mat3 mMatrix);

        // Setting 4x4 matrices
        void SetUniform(std::string sName, glm::mat4* mMatrices, int iCount = 1);
        void SetUniform(std::string sName, const glm::mat4 mMatrix);

        // Setting integers
        void SetUniform(std::string sName, int* iValues, int iCount = 1);
        void SetUniform(std::string sName, const int iValue);
    
    private:
        GLuint handle_;
    };

    
}
