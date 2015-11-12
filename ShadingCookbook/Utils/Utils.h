#pragma once
#include <fstream>
#include <sstream>
#include <iostream>

const char* loadShaderAsString(const char* filename)
{
	std::ifstream fin;
	int fileSize;
	char input;
	char* buffer;

	fin.open(filename);

	if (fin.fail())
		return 0;

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
		return 0;

	fin.open(filename);
	fin.read(buffer, fileSize);

	fin.close();
	buffer[fileSize] = '\0';
	return buffer;
}
