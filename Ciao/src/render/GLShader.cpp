#include "pch.h"
#include "GLShader.h"

#include "utils/ReadFile.h"

namespace Ciao
{
    GLShader::GLShader(const char* fileName)
        : GLShader(GetShaderTypeFromFileName(fileName), ReadShaderFile(fileName).c_str(), fileName)
    {
    }

    GLShader::GLShader(GLenum type, const char* text, const char* debugFileName)
        : type_(type), handle_(glCreateShader(type))
    {
        glShaderSource(handle_, 1, &text, nullptr);
        glCompileShader(handle_);

        char buffer[8192];
        GLsizei length = 0;
        glGetShaderInfoLog(handle_, sizeof(buffer), &length, buffer);

        if (length)
        {
            CIAO_CORE_ERROR("{} (File: {})", buffer, debugFileName);
            CIAO_ASSERT(false, "Compile shader FAILED!");
        }
    }

    GLShader::~GLShader()
    {
        glDeleteShader(handle_);
    }

    void printProgramInfoLog(GLuint handle)
    {
        char buffer[8192];
        GLsizei length = 0;
        glGetProgramInfoLog(handle, sizeof(buffer), &length, buffer);
        if (length)
        {
            CIAO_CORE_ERROR("{}.", buffer);
            CIAO_ASSERT(false, "Shader program link failed!");
        }
    }

    GLProgram::GLProgram(const GLShader& a)
        : handle_(glCreateProgram())
    {
        glAttachShader(handle_, a.getHandle());
        glLinkProgram(handle_);
        printProgramInfoLog(handle_);
    }

    GLProgram::GLProgram(const GLShader& a, const GLShader& b)
        : handle_(glCreateProgram())
    {
        glAttachShader(handle_, a.getHandle());
        glAttachShader(handle_, b.getHandle());
        glLinkProgram(handle_);
        printProgramInfoLog(handle_);
    }

    GLProgram::GLProgram(const GLShader& a, const GLShader& b, const GLShader& c)
        : handle_(glCreateProgram())
    {
        glAttachShader(handle_, a.getHandle());
        glAttachShader(handle_, b.getHandle());
        glAttachShader(handle_, c.getHandle());
        glLinkProgram(handle_);
        printProgramInfoLog(handle_);
    }

    GLProgram::GLProgram(const GLShader& a, const GLShader& b, const GLShader& c, const GLShader& d, const GLShader& e)
        : handle_(glCreateProgram())
    {
        glAttachShader(handle_, a.getHandle());
        glAttachShader(handle_, b.getHandle());
        glAttachShader(handle_, c.getHandle());
        glAttachShader(handle_, d.getHandle());
        glAttachShader(handle_, e.getHandle());
        glLinkProgram(handle_);
        printProgramInfoLog(handle_);
    }

    GLProgram::~GLProgram()
    {
        glDeleteProgram(handle_);
    }

    void GLProgram::useProgram() const
    {
        glUseProgram(handle_);
    }

    /// 设置向量
    void GLProgram::SetUniform(std::string sName, glm::vec2* vVectors, int iCount)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniform2fv(Loc, iCount, (GLfloat*)vVectors);
    }

    void GLProgram::SetUniform(std::string sName, const glm::vec2 vVector)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniform2fv(Loc, 1, (GLfloat*)&vVector);
    }

    void GLProgram::SetUniform(std::string sName, glm::vec3* vVectors, int iCount)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniform3fv(Loc, iCount, (GLfloat*)vVectors);
    }

    void GLProgram::SetUniform(std::string sName, const glm::vec3 vVector)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniform3fv(Loc, 1, (GLfloat*)&vVector);
    }

    void GLProgram::SetUniform(std::string sName, glm::vec4* vVectors, int iCount)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniform4fv(Loc, iCount, (GLfloat*)vVectors);
    }

    void GLProgram::SetUniform(std::string sName, const glm::vec4 vVector)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniform4fv(Loc, 1, (GLfloat*)&vVector);
    }

    /// 设置浮点数值
    void GLProgram::SetUniform(std::string sName, float* fValues, int iCount)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniform1fv(Loc, iCount, (GLfloat*)fValues);
    }

    void GLProgram::SetUniform(std::string sName, const float fValue)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniform1fv(Loc, 1, (GLfloat*)&fValue);
    }

    /// 设置整数值
    void GLProgram::SetUniform(std::string sName, int* iValues, int iCount)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniform1iv(Loc, iCount, iValues);
    }

    void GLProgram::SetUniform(std::string sName, const int iValue)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniform1i(Loc, iValue);
    }

    /// 设置矩阵
    void GLProgram::SetUniform(std::string sName, glm::mat3* mMatrices, int iCount)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniformMatrix3fv(Loc, iCount, GL_FALSE, (GLfloat*)mMatrices);
    }

    void GLProgram::SetUniform(std::string sName, const glm::mat3 mMatrix)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniformMatrix3fv(Loc, 1, GL_FALSE, (GLfloat*)&mMatrix);
    }

    void GLProgram::SetUniform(std::string sName, glm::mat4* mMatrices, int iCount)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniformMatrix4fv(Loc, iCount, GL_FALSE, (GLfloat*)mMatrices);
    }

    void GLProgram::SetUniform(std::string sName, const glm::mat4 mMatrix)
    {
        int Loc = glGetUniformLocation(handle_, sName.c_str());
        glUniformMatrix4fv(Loc, 1, GL_FALSE, (GLfloat*)&mMatrix);
    }
}
