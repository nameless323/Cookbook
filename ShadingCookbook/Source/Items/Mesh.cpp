#include "Mesh.h"

#define uint unsigned int;
#include <fstream>
#include <iostream>

//using std::ifstream;

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