#include "ShaderProgram.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

namespace ShadingCookbook
{
using std::ifstream;
using std::ios;
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
    {".tesc", Shaders::TESS_CONTROL},
    {".tese", Shaders::TESS_EVALUATION},
    {".fs", Shaders::FRAGMENT},
    {".frag", Shaders::FRAGMENT},
    {".comp", Shaders::COMPUTE}
};
}

ShaderProgram::ShaderProgram() : _handle(0), _linked(false)
{
}

ShaderProgram::~ShaderProgram()
{
    if (_handle == 0) return;

    glDeleteProgram(_handle);
}

void ShaderProgram::CompileShader(const std::string& fileName) throw(ShaderProgram)
{
    int numExt = sizeof(ShaderInfo::Extensions) / sizeof(ShaderInfo::ShaderFileExtension);

    std::string ext = GetExtension(fileName.c_str());
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

void ShaderProgram::CompileShader(const std::string& fileName, Shaders::ShaderType type) throw (ShaderProgramException)
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
        CompileShader(buffer, type, fileName.c_str());
    }
    catch (ShaderProgramException e)
    {
        if (buffer != nullptr)
        {
            delete[] buffer;
            buffer = nullptr;
        }
        throw e;
    }

    if (buffer != nullptr)
    {
        delete[] buffer;
        buffer = nullptr;
    }
}

void ShaderProgram::CompileShader(const char* source, Shaders::ShaderType type, const char* fileName) throw (ShaderProgramException)
{
    if (_handle <= 0)
    {
        _handle = glCreateProgram();
        if (_handle == 0)
        {
            throw ShaderProgramException("Unable to create shader program");
        }
    }
    GLuint shaderHandle = glCreateShader(type);
    glShaderSource(shaderHandle, 1, &source, nullptr);

    glCompileShader(shaderHandle);

    int result;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int len = 0;
        std::string logString;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &len);
        if (len > 0)
        {
            char* log = new char[len];
            int written = 0;
            glGetShaderInfoLog(shaderHandle, len, &written, log);
            logString = log;
            delete[] log;
        }
        std::string msg;
        if (fileName != nullptr)
        {
            msg = std::string(fileName) + ": shader compilation failed\n";
        }
        else
        {
            msg = "Shader compilation failed.\n";
        }
        msg += logString;
        throw ShaderProgramException(msg);
    }
    glAttachShader(_handle, shaderHandle);
}

void ShaderProgram::Link()
{
    if (_linked) return;
    if (_handle <= 0)
        throw ShaderProgramException("Program has not been compiled");
    glLinkProgram(_handle);

    int status = 0;
    glGetProgramiv(_handle, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        int len = 0;
        std::string logString;

        glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &len);
        if (len > 0)
        {
            char* log = new char[len];
            int written = 0;
            glGetProgramInfoLog(_handle, len, &written, log);
            logString = log;
            delete[] log;
        }
        throw ShaderProgramException(std::string("Program link failed:\n") + logString);
    }

    GLint numShaders = 0;
    glGetProgramiv(_handle, GL_ATTACHED_SHADERS, &numShaders);

    GLuint* shaderNames = new GLuint[numShaders];
    glGetAttachedShaders(_handle, numShaders, nullptr, shaderNames);

    for (int i = 0; i < numShaders; i++)
    {
        glDeleteShader(shaderNames[i]);
    }
    delete[] shaderNames;
    _uniformLocations.clear();
    _linked = true;
}

void ShaderProgram::Use() throw(ShaderProgramException)
{
    if (_handle <= 0 || !_linked)
        throw ShaderProgramException("Shader has not bene linked");
    glUseProgram(_handle);
}

int ShaderProgram::GetHandle()
{
    return _handle;
}

bool ShaderProgram::IsLinked()
{
    return _linked;
}

void ShaderProgram::BindAttribLocation(GLuint location, const char* name)
{
    glBindAttribLocation(_handle, location, name);
}

void ShaderProgram::BindFragDataLocation(GLuint location, const char* name)
{
    glBindFragDataLocation(_handle, location, name);
}

void ShaderProgram::SetUniform(const char* name, float x, float y, float z)
{
    GLint loc = GetUniformLocation(name);
    glUniform3f(loc, x, y, z);
}

void ShaderProgram::SetUniform(const char* name, const vec3& v)
{
    SetUniform(name, v.x, v.y, v.z);
}

void ShaderProgram::SetUniform(const char* name, const vec4& v)
{
    GLuint loc = GetUniformLocation(name);
    glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void ShaderProgram::SetUniform(const char* name, const vec2& v)
{
    GLuint loc = GetUniformLocation(name);
    glUniform2f(loc, v.x, v.y);
}

void ShaderProgram::SetUniform(const char* name, const mat4& m)
{
    GLuint loc = GetUniformLocation(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

void ShaderProgram::SetUniform(const char* name, const mat3& m)
{
    GLuint loc = GetUniformLocation(name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

void ShaderProgram::SetUniform(const char* name, const float val)
{
    GLuint loc = GetUniformLocation(name);
    glUniform1f(loc, val);
}

void ShaderProgram::SetUniform(const char* name, const int val)
{
    GLuint loc = GetUniformLocation(name);
    glUniform1i(loc, val);
}

void ShaderProgram::SetUniform(const char* name, const GLuint val)
{
    GLuint loc = GetUniformLocation(name);
    glUniform1ui(loc, val);
}

void ShaderProgram::SetUniform(const char* name, const bool val)
{
    GLuint loc = GetUniformLocation(name);
    glUniform1i(loc, val);
}


void ShaderProgram::PrintActiveUniforms()
{
    GLint numUniforms = 0;
    glGetProgramInterfaceiv(_handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

    GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX};

    std::cout << "Active uniforms:" << std::endl;

    for (int i = 0; i < numUniforms; ++i)
    {
        GLint results[4];
        glGetProgramResourceiv(_handle, GL_UNIFORM, i, 4, properties, 4, nullptr, results);

        if (results[3] != -1) continue;

        GLint nameBufSize = results[0] + 1;
        char* name = new char[nameBufSize];
        glGetProgramResourceName(_handle, GL_UNIFORM, i, nameBufSize, nullptr, name);
        std::cout << "\t" << results[2] << " " << name << " (" << GetTypeString(results[1]) << ")" << std::endl;
        delete[] name;
    }
}

void ShaderProgram::PrintActiveUniformBlocks()
{
    GLint numBlocks = 0;

    glGetProgramInterfaceiv(_handle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
    GLenum blockProps[] = {GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH};
    GLenum blockIndex[] = {GL_ACTIVE_VARIABLES};
    GLenum props[] = {GL_NAME_LENGTH, GL_TYPE, GL_BLOCK_INDEX};
    for (int block = 0; block < numBlocks; ++block)
    {
        GLint blockInfo[2];
        glGetProgramResourceiv(_handle, GL_UNIFORM_BLOCK, block, 2, blockProps, 2, nullptr, blockInfo);
        GLint numUnits = blockInfo[0];
        char* blockName = new char[blockInfo[1] + 1];
        glGetProgramResourceName(_handle, GL_UNIFORM_BLOCK, block, blockInfo[1] + 1, nullptr, blockName);
        std::cout << "Uniform block : " << blockName;
        delete[] blockName;

        GLint* unifIndexes = new GLint[numUnits];
        glGetProgramResourceiv(_handle, GL_UNIFORM_BLOCK, block, 1, blockIndex, numUnits, nullptr, unifIndexes);
        for (int unif = 0; unif < numUnits; ++unif)
        {
            GLint uniIndex = unifIndexes[unif];
            GLint results[3];
            glGetProgramResourceiv(_handle, GL_UNIFORM, uniIndex, 3, props, 3, nullptr, results);

            GLint nameBufSize = results[0] + 1;
            char* name = new char[nameBufSize];
            glGetProgramResourceName(_handle, GL_UNIFORM, uniIndex, nameBufSize, nullptr, name);
            std::cout << std::endl << name << " (" << GetTypeString(results[1]) << ") " << std::endl;
            delete[] name;
        }
        delete[] unifIndexes;
    }
}

void ShaderProgram::PrintActiveAttibs()
{
    GLint numAttribs;
    glGetProgramInterfaceiv(_handle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);

    GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION};
    std::cout << "Active attributes:" << std::endl;
    for (int i = 0; i < numAttribs; ++i)
    {
        GLint results[3];
        glGetProgramResourceiv(_handle, GL_PROGRAM_INPUT, i, 3, properties, 3, nullptr, results);

        GLint nameBuffSize = results[0] + 1;
        char* name = new char[nameBuffSize];
        glGetProgramResourceName(_handle, GL_PROGRAM_INPUT, i, nameBuffSize, nullptr, name);
        std::cout << "\t" << results[2] << " " << name << " (" << GetTypeString(results[1]) << std::endl;
        delete[] name;
    }
}

const char* ShaderProgram::GetTypeString(GLenum type)
{
    switch (type)
    {
        case GL_FLOAT:
            return "float";
        case GL_FLOAT_VEC2:
            return "vec2";
        case GL_FLOAT_VEC3:
            return "vec3";
        case GL_FLOAT_VEC4:
            return "vec4";
        case GL_DOUBLE:
            return "double";
        case GL_INT:
            return "int";
        case GL_UNSIGNED_INT:
            return "unsigned int";
        case GL_BOOL:
            return "bool";
        case GL_FLOAT_MAT2:
            return "mat2";
        case GL_FLOAT_MAT3:
            return "mat3";
        case GL_FLOAT_MAT4:
            return "mat4";
        default:
            return "?";
    }
}

void ShaderProgram::Validate() throw(ShaderProgramException)
{
    if (!_linked)
        throw ShaderProgramException("Program is not linked");
    GLint status;
    glValidateProgram(_handle);
    glGetProgramiv(_handle, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE)
    {
        int len = 0;
        std::string logString;

        glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &len);

        if (len > 0)
        {
            char* log = new char[len];
            int written = 0;
            glGetProgramInfoLog(_handle, len, &written, log);
            logString = log;
            delete[] log;
        }
        throw ShaderProgramException(std::string("Program failed to validate\n") + logString);
    }
}

GLint ShaderProgram::GetUniformLocation(const char* name)
{
    std::map<std::string, int>::iterator pos;
    pos = _uniformLocations.find(name);
    if (pos == _uniformLocations.end())
        _uniformLocations[name] = glGetUniformLocation(_handle, name); //what if not

    return _uniformLocations[name];
}

bool ShaderProgram::IsFileExists(const std::string& fileName)
{
    struct stat info;
    int ret = -1;

    ret = stat(fileName.c_str(), &info);
    return 0 == ret;
}
}
