#include "pch.h"
#include "ReadFile.h"

namespace Ciao
{
    bool EndsWith(const std::string &value, const std::string &ending) {
        if (ending.size() > value.size())
            return false;
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    }

    GLenum GetShaderTypeFromFileName(const char* fileName)
    {
        if (EndsWith(fileName, ".vert"))
            return GL_VERTEX_SHADER;

        if (EndsWith(fileName, ".frag"))
            return GL_FRAGMENT_SHADER;

        if (EndsWith(fileName, ".geom"))
            return GL_GEOMETRY_SHADER;

        if (EndsWith(fileName, ".tesc"))
            return GL_TESS_CONTROL_SHADER;

        if (EndsWith(fileName, ".tese"))
            return GL_TESS_EVALUATION_SHADER;

        if (EndsWith(fileName, ".comp"))
            return GL_COMPUTE_SHADER;

        CIAO_ASSERT(false, "Not support shader type.");

        return 0;
    }

    std::string ReadShaderFile(const char* fileName)
    {
        FILE* file;
        fopen_s(&file, fileName, "rt");

        if (!file)
        {
            CIAO_CORE_ERROR("I/O error. Cannot open shader file {}", fileName);
            return std::string();
        }

        fseek(file, 0L, SEEK_END);
        const auto bytesinfile = ftell(file);
        fseek(file, 0L, SEEK_SET);

        char* buffer = (char*)alloca(bytesinfile + 1);
        const size_t bytesread = fread(buffer, 1, bytesinfile, file);
        fclose(file);

        // c 风格字符串结尾标志
        buffer[bytesread] = 0;

        // shader 文件 以 utf - 8 的编码保存，并添加 BOM
        static constexpr unsigned char BOM[] = {0xEF, 0xBB, 0xBF};

        if (bytesread > 3)
        {
            if (!memcmp(buffer, BOM, 3))
                memset(buffer, ' ', 3);
        }

        std::string code(buffer);

        std::string sDirectory(fileName);
        sDirectory = sDirectory.substr(0, sDirectory.find_last_of('/') + 1);


        while (code.find("#include ") != code.npos)
        {
            const auto pos = code.find("#include ");
            const auto p1 = code.find('<', pos);
            const auto p2 = code.find('>', pos);

            if (p1 == code.npos || p2 == code.npos || p2 <= p1)
            {
                CIAO_CORE_ERROR("Error while loading shader program: {}.", code.c_str());
                return std::string();
            }

            const std::string name = sDirectory + code.substr(p1 + 1, p2 - p1 - 1);
            const std::string include = ReadShaderFile(name.c_str());
            code.replace(pos, p2 - pos + 1, include.c_str());
        }

        return code;
    }
}
