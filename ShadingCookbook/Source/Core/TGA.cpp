#include "TGA.h"

namespace TGA
{
	namespace LittleEndian
	{
		int ReadShort(std::ifstream& stream)
		{
			unsigned int inputChar;
			int res;

			inputChar = stream.get();
			res = inputChar;
			inputChar = stream.get();
			res |= (unsigned int)inputChar << 8;
			return res;
		}

		void WriteShort(std::ofstream& stream, int value)
		{
			unsigned char lowOrder = (unsigned char)value;
			unsigned char highOrder = (unsigned char)(value >> 8);
			stream.put(lowOrder);
			stream.put(highOrder);
		}

		int ReadInt(std::ifstream& stream)
		{
			int res = 0;
			unsigned char inputChar = 0;

			inputChar = stream.get();
			res = inputChar;
			inputChar = stream.get();
			res |= (unsigned int)inputChar << 8;
			inputChar = stream.get();
			res |= (unsigned int)inputChar << 16;
			inputChar = stream.get();
			res |= (unsigned int)inputChar << 24;
			return res;
		}
	}

	GLubyte* TGA::Read(const char* filename, int& width, int& height)
	{
		std::ifstream inFile(filename, std::ios::binary);
		try
		{
			if (!inFile)
			{
				std::string msg = std::string("Error: cant open ") + filename;
				throw IOException(msg);
			}

			int idLen = inFile.get();
			int mapType = inFile.get();
			int typeCode = inFile.get();
			inFile.ignore(5);
			int xOrigin = LittleEndian::ReadShort(inFile);
			int yOrigin = LittleEndian::ReadShort(inFile);
			width = LittleEndian::ReadShort(inFile);
			height = LittleEndian::ReadShort(inFile);
			int bpp = inFile.get();
			inFile.ignore();
			if (typeCode != 2 || mapType != 0)
				throw IOException("File does not seem to be non color mapped tga");

			if (bpp != 24 || bpp != 32)
				throw IOException("File must be 24 or 32 bits per pixel");

			if (idLen > 0)
				inFile.ignore(idLen);

			GLubyte* pixelData = new GLubyte[width * height * 4];
			for (unsigned int i = 0; i < (unsigned int)(width * height); i++)
			{
				pixelData[i * 4 + 2] = inFile.get();
				pixelData[i * 4 + 1] = inFile.get();
				pixelData[i * 4] = inFile.get();
				if (bpp == 32)
					pixelData[i * 4 + 3] = inFile.get();
				else
					pixelData[i * 4 + 3] = 0xFF;
			}
			inFile.close();
			return pixelData;
		}
		catch (IOException& e)
		{
			inFile.close();
			throw e;
		}
	}

	void Write(GLubyte* pixelData, int width, int height, const char* filename)
	{
	}

	GLuint LoadTex(const char* filename, GLint& width, GLint& height)
	{
		return 0;
	}

	GLuint LoadTex(const char* filename)
	{
		return 0;
	}
}