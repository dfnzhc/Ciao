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


	GLenum GLShaderTypeFromFileName(const char* fileName)
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

	GLBuffer::GLBuffer(GLsizeiptr size, const void* data, GLbitfield flags)
    {
    	glCreateBuffers(1, &handle_);
    	glNamedBufferStorage(handle_, size, data, flags);
    }

	GLBuffer::~GLBuffer()
    {
    	glDeleteBuffers(1, &handle_);
    }
}
