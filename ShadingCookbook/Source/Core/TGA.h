#pragma once

#include <stdexcept>
#include "GL/glew.h"

namespace ShadingCookbook {namespace TGA
{
class IOException : public std::runtime_error
{
public:
    IOException(const std::string& msg) : std::runtime_error(msg)
    {
    }
};

namespace LittleEndian
{
int ReadShort(std::ifstream& stream);
void WriteShort(std::ofstream& stream, int value);

int ReadInt(std::ifstream& stream);
}

GLubyte* Read(const char* filename, int& width, int& height);
void Write(GLubyte* pixelData, int width, int height, const char* filename);

GLuint LoadTex(const char* filename, GLint& width, GLint& height);

GLuint LoadTex(const char* filename);
}}
