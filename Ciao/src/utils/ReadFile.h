#pragma once

namespace Ciao
{
    // 判断字符是否以 ending 结尾
    bool EndsWith(const std::string &value, const std::string &ending);

    // 从文件名判断着色器类型
    GLenum GetShaderTypeFromFileName(const char* fileName);

    // 读取着色器文件
    std::string ReadShaderFile(const char* fileName);

    // 替换字符串中的子串
    std::string ReplaceAll(const std::string& str, const std::string& oldSubStr, const std::string& newSubStr);

    // 将字符串转换为小写
    std::string LowercaseString(const std::string& s);

    // 朝文件中保存字符串数组
    void SaveStringList(FILE* f, const std::vector<std::string>& lines);

    // 从文件中读取字符串数组
    void LoadStringList(FILE* f, std::vector<std::string>& lines);

    // 唯一地加入字符串，然后返回它所在的位置
    inline int AddUnique(std::vector<std::string>& files, const std::string& file);

    void ReadSceneFile(const char* fileName);
}
