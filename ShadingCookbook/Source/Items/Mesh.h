//
// Mesh loader.
//

#pragma once

#include "GL/glew.h"
#include <vector>
#include <glm.hpp>

#include "IDrawable.h"

namespace ShadingCookbook
{
using std::string;
using glm::vec3;
using glm::vec4;
using glm::vec2;
using std::vector;

class Mesh : public IDrawable
{
public:
    /**
     * \brief Load mesh from file with additional parameters.
     */
    Mesh(const char* filename, bool reCenterMesh = false, bool loadUv = false, bool genTangents = false);
    /**
     * \brief Render mesh.
     */
    void Render() override;
    /**
     * \brief Load mesh from file.
     */
    void LoadObj(const char* filename);

private:
    GLuint _faces;
    GLuint _vao;

    bool _recenterMesh, _loadTex, _genTang;
    
    /**
     * \brief Trim string.
     */
    void TrimString(string& str);
    /**
     * \brief Stores vertex buffer object.
     */
    void StoreVbo(const vector<vec3>& points, const vector<vec3>& normals,
                  const vector<vec2>& texCoords, const vector<vec4>& tangents, const vector<GLuint>& elements);
    /**
     * \brief Generate average normals.
     */
    void GenerateAverageNormals(const vector<vec3>& points, vector<vec3>& normals, const vector<GLuint>& faces);
    /**
     * \brief Generage mesh tangents from uv.
     */
    void GenerateTangents(const vector<vec3>& points, const vector<vec3>& normals, const vector<GLuint>& faces, const vector<vec2>& texCoords, vector<vec4>& tangents);
    /**
     * \brief Center mesh.
     */
    void Center(vector<vec3>& points);

};
}