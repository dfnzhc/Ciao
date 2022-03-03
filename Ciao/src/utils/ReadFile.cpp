#include "pch.h"
#include "ReadFile.h"

#include "render/Scene.h"

namespace Ciao
{
    bool EndsWith(const std::string& value, const std::string& ending)
    {
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

    std::string ReplaceAll(const std::string& str, const std::string& oldSubStr, const std::string& newSubStr)
    {
        std::string result = str;

        for (size_t p = result.find(oldSubStr); p != std::string::npos; p = result.find(oldSubStr))
            result.replace(p, oldSubStr.length(), newSubStr);

        return result;
    }

    std::string LowercaseString(const std::string& s)
    {
        std::string out(s.length(), ' ');
        std::transform(std::execution::par, s.begin(), s.end(), out.begin(), tolower);
        return out;
    }

    void SaveStringList(FILE* f, const std::vector<std::string>& lines)
    {
        uint32_t sz = (uint32_t)lines.size();

        fwrite(&sz, sizeof(uint32_t), 1, f);
        for (const auto& s : lines)
        {
            sz = (uint32_t)s.length();
            fwrite(&sz, sizeof(uint32_t), 1, f);
            fwrite(s.c_str(), sz + 1, 1, f);
        }
    }

    void LoadStringList(FILE* f, std::vector<std::string>& lines)
    {
        {
            uint32_t sz = 0;
            fread(&sz, sizeof(uint32_t), 1, f);
            lines.resize(sz);
        }
        std::vector<char> inBytes;
        for (auto& s : lines)
        {
            uint32_t sz = 0;
            fread(&sz, sizeof(uint32_t), 1, f);
            inBytes.resize(sz + 1);
            fread(inBytes.data(), sz + 1, 1, f);
            s = std::string(inBytes.data());
        }
    }

    inline int AddUnique(std::vector<std::string>& files, const std::string& file)
    {
        if (file.empty())
            return -1;

        auto i = std::find(std::begin(files), std::end(files), file);

        if (i == files.end())
        {
            files.push_back(file);
            return (int)files.size() - 1;
        }

        return (int)std::distance(files.begin(), i);
    }

    void ReadSceneFile(const char* fileName)
    {
        
    }
}
