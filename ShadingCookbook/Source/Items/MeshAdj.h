#pragma once

#include "IDrawable.h"

#include <Vector>
using std::vector;
#include <glm.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;

#include <string>
using std::string;
#include <GL/glew.h>

class MeshAdj : public IDrawable
{
public:
    MeshAdj(const char* filename, bool reCenterMesh = false);
    void Render() override;
    void LoadObj(const char* filename, bool recenterMesh);
private:
    GLuint _faces;
    GLuint _vao;

    void TrimString(string& str);
    void DetermineAdjacency(vector<GLuint>& el);
    void StoreVBO(const vector<vec3>& points, const vector<vec3>& normals, const vector<vec2>& texCoords, const vector<vec4>& tangents, const vector<GLuint>& elements);
    void GenerateAveragedNormals(const vector<vec3>& points, vector<vec3>& normals, const vector<GLuint>& faces);
    void GenerateTangents(const vector<vec3>& points, const vector<vec3>& normals, const vector<GLuint>& faces, const vector<vec2>& texCoords, vector<vec4>& tangents);
    void Center(vector<vec3>& points);
};



