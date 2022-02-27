#pragma once

namespace Ciao
{
    bool EndsWith(const std::string &value, const std::string &ending);

    GLenum GetShaderTypeFromFileName(const char* fileName);
    
    std::string ReadShaderFile(const char* fileName);
}
