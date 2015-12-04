#include "Mesh.h"

#define uint unsigned int;
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using std::ifstream;

Mesh::Mesh(const char* filename, bool reCenterMesh, bool loadUv, bool genTangents) : _recenterMesh(reCenterMesh), _loadTex(loadUv), _genTang(genTangents)
{
	LoadObj(filename);
}

void Mesh::Render()
{
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, 3 * _faces, GL_UNSIGNED_INT, (GLubyte*)NULL + 0);
	glBindVertexArray(0);
}

void Mesh::LoadObj(const char* filename)
{
	vector<vec3> points;
	vector<vec3> normals;
	vector<vec2> uv;
	vector<GLuint> faces;

	int nFaces = 0;
	std::ifstream objStream(filename);
	if (!objStream)
	{
		std::cerr << "unable to open obj file: " << filename << std::endl;
	}
	string line, token;
	vector<int> face;
	std::getline(objStream, line);
	while(!objStream.eof())
	{
		TrimString(line);
		if (line.length() > 0 && line.at(0) != '#')
		{
			std::istringstream lineStream(line);

			lineStream >> token;
			if (token == "v")
			{
				float x, y, z;
				lineStream >> x >> y >> z;
				points.push_back(vec3(x, y, z));
			}
			else if (token == "vt" && _loadTex)
			{
				float s, t;
				lineStream >> s >> t;
				uv.push_back(vec2(s, t));
			}
			else if (token == "vn")
			{
				float x, y, z;
				lineStream >> x >> y >> z;
				normals.push_back(vec3(x, y, z));
			}
			else if (token == "f")
			{
				nFaces++;

				face.clear();
				size_t slash1, slash2;
				while (lineStream.good())
				{
					string vertString;
					lineStream >> vertString;
					int pIndex = -1, nIndex = -1, tcIndex = -1;

					slash1 = vertString.find("/");
					if (slash1 == string::npos)
					{
						pIndex = atoi(vertString.c_str()) - 1;
					}
					else
					{
						slash2 = vertString.find("/", slash1 + 4);
						pIndex = atoi(vertString.substr(0, slash1).c_str()) - 1;
						if (slash2 > slash1 + 1)
						{
							tcIndex = atoi(vertString.substr(slash1 + 1, slash2).c_str()) - 1;
						}
						nIndex = atoi(vertString.substr(slash2 + 1, vertString.length()).c_str()) - 1;
					}
				}
			}
		}
	}
}

void Mesh::TrimString(string& str)
{
}

void Mesh::StoreVbo(const std::vector<vec3>& points, const std::vector<vec3>& normals, const std::vector<vec2>& texCoords, const std::vector<vec4>& tangents, const std::vector<GLuint>& elements)
{
}

void Mesh::GenerateAverageNormals(const std::vector<vec3>& points, std::vector<vec3>& normals, const std::vector<GLuint>& faces)
{
}

void Mesh::GenerateTangents(const std::vector<vec3>& points, const std::vector<vec3>& normals, const std::vector<GLuint>& texcoords, std::vector<vec4>& tangents)
{
}

void Mesh::Center(std::vector<vec4>& points)
{
}