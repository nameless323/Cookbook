#include "ShaderProgram.h"

#include <fstream>
using std::ifstream;
using std::ios;

#include <sstream>
#include <sys/stat.h>

namespace ShaderInfo
{
	struct ShaderFileExtension
	{
		const char* ext;
		Shaders::ShaderType type;
	};

	struct ShaderFileExtension Extensions[]
	{
		{".vs", Shaders::VERTEX},
		{".vert", Shaders::VERTEX},
		{".gs", Shaders::GEOMETRY},
		{".geom", Shaders::GEOMETRY},
		{".tcs", Shaders::TESS_CONTROL},
		{".tes", Shaders::TESS_EVALUATION},
		{".fs", Shaders::FRAGMENT},
		{".frag", Shaders::FRAGMENT},
		{".cs", Shaders::COMPUTE}
	};
}

ShaderProgram::ShaderProgram() : _handle(0), _linked(false) {}

ShaderProgram::~ShaderProgram()
{
	if (_handle == 0) return;

	GLint numShaders = 0;
	glGetProgramiv(_handle, GL_ATTACHED_SHADERS, &numShaders);

	GLuint* shaderNames = new GLuint[numShaders];
	glGetAttachedShaders(_handle, numShaders, NULL, shaderNames);

	for (int i = 0; i < numShaders; i++)
	{
		glDeleteShader(shaderNames[i]);
	}

	glDeleteProgram(_handle);

	delete[] shaderNames;
}

void ShaderProgram::CompileShader(const char* fileName) throw(ShaderProgram)
{
	int numExt = sizeof(ShaderInfo::Extensions) / sizeof(ShaderInfo::ShaderFileExtension);

	std::string ext = GetExtension(fileName);
	Shaders::ShaderType type = Shaders::VERTEX;
	bool matchFound = false;
	for (int i = 0; i < numExt; i++)
	{
		if (ext == ShaderInfo::Extensions[i].ext)
		{
			matchFound = true;
			type = ShaderInfo::Extensions[i].type;
			break;
		}
	}
	if (!matchFound)
	{
		std::string msg = "Unrecognized shader extension " + ext;
		throw ShaderProgramException(msg);
	}

	CompileShader(fileName, type);
}

std::string ShaderProgram::GetExtension(const char* fileName)
{
	std::string nameStr(fileName);
	size_t loc = nameStr.find_last_of('.');
	if (loc != std::string::npos)
		return nameStr.substr(loc, std::string::npos);
	return "";
}

void ShaderProgram::CompileShader(const char* fileName, Shaders::ShaderType type) throw (ShaderProgramException)
{
	if (!IsFileExists(fileName))
	{
		std::string message = std::string("Shader: ") + fileName + " not found.";
		throw ShaderProgramException(message);
	}
	if (_handle <= 0)
	{
		_handle = glCreateProgram();
		if (_handle == 0)
			throw ShaderProgramException("Unable to create shader program");
	}

	std::ifstream fin;
	int fileSize;
	char input;
	char* buffer;

	fin.open(fileName);

	if (fin.fail())
	{
		std::string message = std::string("Shader: ") + fileName + " not found.";
		throw ShaderProgramException(message);
	}

	fileSize = 0;
	fin.get(input);

	while (!fin.eof())
	{
		fileSize++;
		fin.get(input);
	}
	fin.close();

	buffer = new char[fileSize + 1];
	if (!buffer)
	{
		std::string message = std::string("Failed to allocate memory for buffer for shader ") + fileName;
		throw ShaderProgramException(message);
	}

	fin.open(fileName);
	fin.read(buffer, fileSize);

	fin.close();
	buffer[fileSize] = '\0';
	try
	{
		CompileShader(buffer, type, fileName);
	} catch(ShaderProgramException e)
	{
		delete[] buffer;
		throw e;
	}

	delete[] buffer;
}
