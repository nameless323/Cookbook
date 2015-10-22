#pragma once
#include <fstream>

const char* loadShaderAsString(const char* file)
{
	std::ifstream shader_file(file, std::ifstream::in);
	std::string str((std::istreambuf_iterator<char>(shader_file)), std::istreambuf_iterator<char>());
	return str.c_str();
}