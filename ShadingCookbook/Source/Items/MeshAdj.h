//
// Mesh loader. Generates adjancy information.
//

#pragma once

#include "IDrawable.h"

#include <Vector>
#include <glm.hpp>
#include <string>
#include <GL/glew.h>

namespace ShadingCookbook
{
using std::vector;

using glm::vec2;
using glm::vec3;
using glm::vec4;

using std::string;

class MeshAdj : public IDrawable
{
public:
    /**
     * \brief Load mesh from file with additional parameters.
     */
    MeshAdj(const char* filename, bool reCenterMesh = false);
    /**
     * \brief Render mesh.
     */
    void Render() override;
    /**
     * \brief Load mesh from file.
     */
    void LoadObj(const char* filename, bool recenterMesh);

private:
    GLuint _faces;
    GLuint _vao;

    /**
     * \brief Trim string.
     */
    void TrimString(string& str);
    /**
     * \brief Determine edges adjancy.
     */
    void DetermineAdjacency(vector<GLuint>& el);
    /**
     * \brief Stores vertex buffer object.
     */
    void StoreVBO(const vector<vec3>& points, const vector<vec3>& normals, const vector<vec2>& texCoords, const vector<vec4>& tangents, const vector<GLuint>& elements);
    /**
     * \brief Generate average normals.
     */
    void GenerateAveragedNormals(const vector<vec3>& points, vector<vec3>& normals, const vector<GLuint>& faces);
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
