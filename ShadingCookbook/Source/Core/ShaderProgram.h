#pragma once
#include <GL/glew.h>
#include <stdexcept>
#include <map>
#include <glm.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;


class ShaderProgramException : public std::runtime_error
{
public:
	ShaderProgramException(const std::string& msg) : std::runtime_error(msg)
	{
	}
};

namespace Shaders
{
	enum ShaderType
	{
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER,
		TESS_CONTROL = GL_TESS_CONTROL_SHADER,
		TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
		COMPUTE = GL_COMPUTE_SHADER
	};
}

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	void CompileShader(const char *fileName) throw(ShaderProgramException);
	void CompileShader(const char* fileName, Shaders::ShaderType type) throw(ShaderProgramException);
	void CompileShader(const std::string& source, Shaders::ShaderType type, const char* fileName = nullptr) throw(ShaderProgramException);

	void Link() throw (ShaderProgramException);
	void Validate() throw (ShaderProgramException);
	void Use() throw (ShaderProgramException);

	int GetHandle();
	bool IsLinked();

	void BindAttribLocation(GLuint location, const char* name);
	void BindFragDataLocation(GLuint location, const char* name);

	void SetUniform(const char* name, float x, float y, float z);
	void SetUniform(const char* name, const vec2& v);
	void SetUniform(const char* name, const vec3& v);
	void SetUniform(const char* name, const vec4& v);
	void SetUniform(const char* name, const mat4& v);
	void SetUniform(const char* name, const mat3& v);
	void SetUniform(const char* name, const float val);
	void SetUniform(const char* name, const int val);
	void SetUniform(const char* name, const bool val);
	void SetUniform(const char* name, const GLuint val);

	void PrintActiveUniforms();
	void PrintActiveUniformBlocks();
	void PrintActiveAttibs();

	const char* GetTypeString(GLenum type);
private:
	int _handle;
	bool _linked;
	std::map<std::string, int> _uniformLocations;
	
	GLint GetUniformLocation(const char* name);
	bool IsFileExists(const std::string& fileName);
	std::string GetExtension(const char* fileName);

	ShaderProgram(const ShaderProgram& other): _handle(0), _linked(false)	{}
	ShaderProgram& operator= (const ShaderProgram& other) { return *this; }
};

