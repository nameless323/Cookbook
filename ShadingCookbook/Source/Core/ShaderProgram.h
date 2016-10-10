//
// Helper for shader program.
//

#pragma once

#include <GL/glew.h>
#include <stdexcept>
#include <map>
#include <glm.hpp>

namespace ShadingCookbook
{
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

    /**
     * \brief Compile shader from file.
     */
    void CompileShader(const std::string& fileName) throw(ShaderProgramException);
    /**
     * \brief Compile shader from file.
     */
    void CompileShader(const std::string& fileName, Shaders::ShaderType type) throw(ShaderProgramException);
    /**
     * \brief Compile shader from file.
     */
    void CompileShader(const char* source, Shaders::ShaderType type, const char* fileName = nullptr) throw(ShaderProgramException);
    /**
     * \brief Link shader.
     */
    void Link() throw (ShaderProgramException);
    /**
     * \brief Validate shader correctness.
     */
    void Validate() throw (ShaderProgramException);
    /**
     * \brief Use shader.
     */
    void Use() throw (ShaderProgramException);
    /**
     * \brief Get shader handle.
     */
    int GetHandle();
    /**
     * \brief Check if shader linked.
     */
    bool IsLinked();
    /**
     * \brief Bind attribute location.
     */
    void BindAttribLocation(GLuint location, const char* name);
    /**
     * \brief Bing fragment data location.
     */
    void BindFragDataLocation(GLuint location, const char* name);
    /**
     * \brief Set shader uniform.
     */
    void SetUniform(const char* name, float x, float y, float z);
    /**
     * \brief Set shader uniform.
     */
    void SetUniform(const char* name, const vec2& v);
    /**
     * \brief Set shader uniform.
     */
    void SetUniform(const char* name, const vec3& v);
    /**
     * \brief Set shader uniform.
     */
    void SetUniform(const char* name, const vec4& v);
    /**
     * \brief Set shader uniform.
     */
    void SetUniform(const char* name, const mat4& v);
    /**
     * \brief Set shader uniform.
     */
    void SetUniform(const char* name, const mat3& v);
    /**
     * \brief Set shader uniform.
     */
    void SetUniform(const char* name, const float val);
    /**
     * \brief Set shader uniform.
     */
    void SetUniform(const char* name, const int val);
    /**
     * \brief Set shader uniform.
     */
    void SetUniform(const char* name, const bool val);
    /**
     * \brief Set shader uniform.
     */
    void SetUniform(const char* name, const GLuint val);

    /**
     * \brief Log active shader uniforms.
     */
    void PrintActiveUniforms();
    /**
     * \brief Log active shader uniform blocks.
     */
    void PrintActiveUniformBlocks();
    /**
     * \brief Log active shader attributes.
     */
    void PrintActiveAttibs();
    /**
     * \brief Get type from string.
     */
    const char* GetTypeString(GLenum type);

private:
    int _handle;
    bool _linked;
    std::map<std::string, int> _uniformLocations;

    /**
     * \brief Get location of the uniform.
     */
    GLint GetUniformLocation(const char* name);
    /**
     * \brief Check if file exist.
     */
    bool IsFileExists(const std::string& fileName);
    /**
     * \brief Get shader extension from filename.
     */
    std::string GetExtension(const char* fileName);

    ShaderProgram(const ShaderProgram& other): _handle(0), _linked(false)
    {
    }

    ShaderProgram& operator=(const ShaderProgram& other) { return *this; }
};
}
