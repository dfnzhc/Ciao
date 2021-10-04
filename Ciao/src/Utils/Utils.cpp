#include "pch.h"
#include "Utils.h"


namespace Ciao
{
	int endsWith(const char* s, const char* part)
	{
		return (strstr(s, part) - s) == (strlen(s) - strlen(part));
	}

	std::string readShaderFile(const char* fileName)
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

			const std::string name = sDirectory.append(code.substr(p1 + 1, p2 - p1 - 1));
			const std::string include = readShaderFile(name.c_str());
			code.replace(pos, p2 - pos + 1, include.c_str());
		}

		return code;
	}


	std::string replaceAll(const std::string& str, const std::string& oldSubStr, const std::string& newSubStr)
	{
		std::string result = str;

		for (size_t p = result.find(oldSubStr); p != std::string::npos; p = result.find(oldSubStr))
			result.replace(p, oldSubStr.length(), newSubStr);

		return result;
	}

	std::string lowercaseString(const std::string& s)
	{
		std::string out(s.length(), ' ');
		std::transform(s.begin(), s.end(), out.begin(), tolower);
		return out;
	}

	void saveStringList(FILE* f, const std::vector<std::string>& lines)
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

	void loadStringList(FILE* f, std::vector<std::string>& lines)
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
}
