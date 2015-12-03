#pragma once
#include "GL/glew.h"
#include "IDrawable.h"

#include <vector>
#include <glm.hpp>
using std::string;
using glm::vec3;
using glm::vec4;
using glm::vec2;
using std::vector;

class Mesh : public IDrawable
{
public:
	Mesh(const char* filename, bool reCenterMesh = false, bool loadUv = false, bool genTangents = false);

	void Render() override;
	void LoadObj(const char* filename);
private:
	void TrimString(string& str);
	void StoreVbo(const vector<vec3>& points, const vector<vec3>& normals,
		const vector<vec2>& texCoords, const vector<vec4>& tangents, const vector<GLuint>& elements);
	void GenerateAverageNormals(const vector<vec3>& points, vector<vec3>& normals, const vector<GLuint>& faces);
	void GenerateTangents(const vector<vec3>& points, const vector<vec3>& normals, const vector<GLuint>& texcoords, vector<vec4>& tangents);
	void Center(vector<vec4>& points);
	
	GLuint _faces;
	GLuint _vao;

	bool _recenterMesh, _loadTex, _genTang;
};