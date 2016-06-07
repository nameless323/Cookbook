#include "MeshAdj.h"

MeshAdj::MeshAdj(const char* filename, bool reCenterMesh)
{
    LoadObj(filename, reCenterMesh);
}

void MeshAdj::Render()
{
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES_ADJACENCY, 6 * _faces, GL_UNSIGNED_INT, ((GLubyte*)NULL + 0));
}

void MeshAdj::LoadObj(const char* filename, bool recenterMesh)
{
}

void MeshAdj::TrimString(string& str)
{
}

void MeshAdj::DetermineAdjacency(vector<GLuint>& el)
{
}

void MeshAdj::StoreVBO(const vector<vec3>& points, const vector<vec3>& normals, const vector<vec2>& texCoords, const vector<vec4>& tangents, const vector<GLuint>& elements)
{
}

void MeshAdj::GenerateAveragedNormals(const vector<vec3>& points, vector<vec3>& normals, const vector<GLuint>& faces)
{
}

void MeshAdj::GenerateTangents(const vector<vec3>& points, const vector<vec3>& normals, const vector<GLuint>& faces, const vector<vec2>& texCoords, vector<vec4>& tangents)
{
}

void MeshAdj::Center(vector<vec3>& points)
{
}