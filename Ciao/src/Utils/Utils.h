#pragma once


namespace Ciao
{
	int endsWith(const char* s, const char* part);

	std::string readShaderFile(const char* fileName);


	constexpr std::string ResourceDir(const std::string& s) { return std::string("Resources/").append(s); }

#define Res(s) ResourceDir(s).c_str()

}
