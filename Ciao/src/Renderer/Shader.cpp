#include "pch.h"
#include "Shader.h"

namespace Ciao
{
    Shader::Shader()
    {
        m_isLoaded = false;
    }

    Shader::Shader(Shader&& shader) noexcept : m_shaderID(shader.m_shaderID), m_ShaderType(shader.m_ShaderType), m_isLoaded(shader.m_isLoaded) 
    {
        shader.m_shaderID = -1;
    }

    Shader::Shader(const std::string& filename)
    {
        std::string sExt = filename.substr((int)filename.size() - 4, 4);
        int iShaderType;
        if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
        else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
        else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
        else if (sExt == "tesc") iShaderType = GL_TESS_CONTROL_SHADER;
        else if (sExt == "tese") iShaderType = GL_TESS_EVALUATION_SHADER;
        else if (sExt == "comp") iShaderType = GL_COMPUTE_SHADER;
        else iShaderType = GL_COMPUTE_SHADER;

        LoadShader(GetAssetDir() + "Shaders/" + filename, iShaderType);
    }
    

    bool Shader::LoadShader(const std::string& sFile, int iType)
    {
        std::vector<std::string> sLines;

        if(!GetLinesFromFile(sFile, false, &sLines)) {
            CIAO_CORE_ERROR("Cannot load shader\n{}\n", sFile.c_str());
            return false;
        }

        const char** sProgram = new const char*[(int)sLines.size()];
        for (int i = 0; i < (int)sLines.size(); i++) 
            sProgram[i] = sLines[i].c_str();
	    
        m_shaderID = glCreateShader(iType);

        glShaderSource(m_shaderID, (int)sLines.size(), sProgram, NULL);
        glCompileShader(m_shaderID);

        delete[] sProgram;

        int iCompilationStatus;
        glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &iCompilationStatus);

        if(iCompilationStatus == GL_FALSE)
        {
            char sInfoLog[1024];
            char sFinalMessage[1536];
            int iLogLength;
            glGetShaderInfoLog(m_shaderID, 1024, &iLogLength, sInfoLog);
            char sShaderType[64];
            if (iType == GL_VERTEX_SHADER)
                sprintf_s(sShaderType, "vertex shader");
            else if (iType == GL_FRAGMENT_SHADER)
                sprintf_s(sShaderType, "fragment shader");
            else if (iType == GL_GEOMETRY_SHADER)
                sprintf_s(sShaderType, "geometry shader");
            else if (iType == GL_TESS_CONTROL_SHADER)
                sprintf_s(sShaderType, "tess control shader");
            else if (iType == GL_TESS_EVALUATION_SHADER)
                sprintf_s(sShaderType, "tess evaluation shader");
            else if (iType == GL_COMPUTE_SHADER)
                sprintf_s(sShaderType, "compute shader");
            else
                sprintf_s(sShaderType, "unknown shader type");

            std::string lineNum{sInfoLog};
            int l, r;
            l = r = 0;
            for (unsigned int i = 0; i < lineNum.size(); ++i) {
                if (lineNum[i] == '(') l = i;
                else if (lineNum[i] == ')') {
                    r = i;
                    break;
                }
            }

            int line = 0;
            if (l != r && l < r) {
                lineNum = lineNum.substr(l + 1, r - l - 1);
                line = atoi(lineNum.c_str()) - 3;
            }

            CIAO_CORE_ERROR("Error happens in {}!\n{}\nShader file can not be compiled. The compiler returned: \n\n[{}]{}", sShaderType, sFile.c_str(), line, sInfoLog);
            return false;
        }
        m_ShaderType = iType;
        m_isLoaded = true;

        return true;
    }

    void Shader::DeleteShader()
    {
        if(!IsLoaded())
            return;
        m_isLoaded = false;
        glDeleteShader(m_shaderID);
    }

    bool Shader::GetLinesFromFile(const std::string& sFile, bool bIncludePart, std::vector<std::string>* vResult)
    {
        FILE* fp;
        fopen_s(&fp, sFile.c_str(), "rt");
        if(!fp)return false;

        std::string sDirectory;
        int slashIndex = -1;

        for (int i = (int)sFile.size()-1; i >= 0; i--)
        {
            if(sFile[i] == '\\' || sFile[i] == '/')
            {
                slashIndex = i;
                break;
            }
        }

        sDirectory = sFile.substr(0, slashIndex+1);

        // Get all lines from a file
        char sLine[255];

        bool bInIncludePart = false;

        while(fgets(sLine, 255, fp))
        {
            std::stringstream ss(sLine);
            std::string sFirst;
            ss >> sFirst;
            if(sFirst == "#include")
            {
                std::string sFileName;
                ss >> sFileName;
                if((int)sFileName.size() > 0 && sFileName[0] == '\"' && sFileName[(int)sFileName.size()-1] == '\"')
                {
                    sFileName = sFileName.substr(1, (int)sFileName.size()-2);
                    GetLinesFromFile(sDirectory+sFileName, true, vResult);
                }
            }
            else if(sFirst == "#include_part")
                bInIncludePart = true;
            else if(sFirst == "#definition_part")
                bInIncludePart = false;
            else if(!bIncludePart || (bIncludePart && bInIncludePart))
                vResult->push_back(sLine);
        }
        fclose(fp);

        return true;
    }

    bool Shader::IsLoaded() const 
    {
        return m_isLoaded;
    }

    glm::uint Shader::GetShaderID() const
    {
        return m_shaderID;
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

    ShaderProgram::ShaderProgram()
    {
        m_isLinked = false;
    }

    ShaderProgram::ShaderProgram(const Shader& a) : m_ProgramID(glCreateProgram())
    {
        glAttachShader(m_ProgramID, a.GetShaderID());
        glLinkProgram(m_ProgramID);
        printProgramInfoLog(m_ProgramID);
    }

    ShaderProgram::ShaderProgram(const Shader& a, const Shader& b) : m_ProgramID(glCreateProgram())
    {
        glAttachShader(m_ProgramID, a.GetShaderID());
        glAttachShader(m_ProgramID, b.GetShaderID());
        glLinkProgram(m_ProgramID);
        printProgramInfoLog(m_ProgramID);
    }

    ShaderProgram::ShaderProgram(const Shader& a, const Shader& b, const Shader& c) : m_ProgramID(glCreateProgram())
    {
        glAttachShader(m_ProgramID, a.GetShaderID());
        glAttachShader(m_ProgramID, b.GetShaderID());
        glAttachShader(m_ProgramID, c.GetShaderID());
        glLinkProgram(m_ProgramID);
        printProgramInfoLog(m_ProgramID);
    }

    ShaderProgram::ShaderProgram(const Shader& a, const Shader& b, const Shader& c, const Shader& d) : m_ProgramID(glCreateProgram())
    {
        glAttachShader(m_ProgramID, a.GetShaderID());
        glAttachShader(m_ProgramID, b.GetShaderID());
        glAttachShader(m_ProgramID, c.GetShaderID());
        glAttachShader(m_ProgramID, d.GetShaderID());
        glLinkProgram(m_ProgramID);
        printProgramInfoLog(m_ProgramID);
    }

    ShaderProgram::ShaderProgram(const Shader& a, const Shader& b, const Shader& c, const Shader& d, const Shader& e) : m_ProgramID(glCreateProgram())
    {
        glAttachShader(m_ProgramID, a.GetShaderID());
        glAttachShader(m_ProgramID, b.GetShaderID());
        glAttachShader(m_ProgramID, c.GetShaderID());
        glAttachShader(m_ProgramID, d.GetShaderID());
        glAttachShader(m_ProgramID, e.GetShaderID());
        glLinkProgram(m_ProgramID);
        printProgramInfoLog(m_ProgramID);
    }

    void ShaderProgram::CreateProgram()
    {
        m_ProgramID = glCreateProgram();
    }

    void ShaderProgram::DeleteProgram()
    {
        if(!m_isLinked)
            return;
        m_isLinked = false;
        glDeleteProgram(m_ProgramID);
    }

    bool ShaderProgram::AddShaderToProgram(const Shader* shShader)
    {
        if (!shShader->IsLoaded())
            return false;

        glAttachShader(m_ProgramID, shShader->GetShaderID());
        return true;
    }

    bool ShaderProgram::LinkProgram()
    {
        glLinkProgram(m_ProgramID);
        int LinkStatus;
        glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &LinkStatus);

        if (LinkStatus == GL_FALSE) {
            char sInfoLog[1024];
            int iLogLength;
            glGetProgramInfoLog(m_ProgramID, 1024, &iLogLength, sInfoLog);
            CIAO_CORE_ERROR("Error! Shader program wasn't linked! The linker returned:\n\n{}", sInfoLog);
            return false;
        }

        m_isLinked = LinkStatus == GL_TRUE;
        return m_isLinked;
    }

    void ShaderProgram::UseProgram()
    {
        if (m_isLinked)
            glUseProgram(m_ProgramID);
    }

    glm::uint ShaderProgram::GetProgramID()
    {
        return m_ProgramID;
    }

    /// 设置向量
    void ShaderProgram::SetUniform(const std::string& sName, glm::vec2* vVectors, int iCount)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniform2fv(Loc, iCount, (GLfloat*)vVectors);
    }

    void ShaderProgram::SetUniform(const std::string& sName, const glm::vec2 vVector)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniform2fv(Loc, 1, (GLfloat*)&vVector);
    }

    void ShaderProgram::SetUniform(const std::string& sName, glm::vec3* vVectors, int iCount)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniform3fv(Loc, iCount, (GLfloat*)vVectors);
    }

    void ShaderProgram::SetUniform(const std::string& sName, const glm::vec3 vVector)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniform3fv(Loc, 1, (GLfloat*)&vVector);
    }

    void ShaderProgram::SetUniform(const std::string& sName, glm::vec4* vVectors, int iCount)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniform4fv(Loc, iCount, (GLfloat*)vVectors);
    }

    void ShaderProgram::SetUniform(const std::string& sName, const glm::vec4 vVector)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniform4fv(Loc, 1, (GLfloat*)&vVector);
    }

    /// 设置浮点数值
    void ShaderProgram::SetUniform(const std::string& sName, float* fValues, int iCount)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniform1fv(Loc, iCount, (GLfloat*)fValues);
    }

    void ShaderProgram::SetUniform(const std::string& sName, const float fValue)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniform1fv(Loc, 1, (GLfloat*)&fValue);
    }

    /// 设置整数值
    void ShaderProgram::SetUniform(const std::string& sName, int* iValues, int iCount)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniform1iv(Loc, iCount, iValues);
    }

    void ShaderProgram::SetUniform(const std::string& sName, const int iValue)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniform1i(Loc, iValue);
    }

    /// 设置矩阵
    void ShaderProgram::SetUniform(const std::string& sName, glm::mat3* mMatrices, int iCount)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniformMatrix3fv(Loc, iCount, GL_FALSE, (GLfloat*)mMatrices);
    }

    void ShaderProgram::SetUniform(const std::string& sName, const glm::mat3 mMatrix)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniformMatrix3fv(Loc, 1, GL_FALSE, (GLfloat*)&mMatrix);
    }

    void ShaderProgram::SetUniform(const std::string& sName, glm::mat4* mMatrices, int iCount)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniformMatrix4fv(Loc, iCount, GL_FALSE, (GLfloat*)mMatrices);
    }

    void ShaderProgram::SetUniform(const std::string& sName, const glm::mat4 mMatrix)
    {
        int Loc = glGetUniformLocation(m_ProgramID, sName.c_str());
        glUniformMatrix4fv(Loc, 1, GL_FALSE, (GLfloat*)&mMatrix);
    }



    GLBuffer::GLBuffer(GLsizeiptr size, const void* data, GLbitfield flags)
    {
        glCreateBuffers(1, &m_Handle);
        glNamedBufferStorage(m_Handle, size, data, flags);
    }

    GLBuffer::~GLBuffer()
    {
        glDeleteBuffers(1, &m_Handle);
    }
}