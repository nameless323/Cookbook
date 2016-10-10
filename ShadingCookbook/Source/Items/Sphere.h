//
// Sphere generator.
//

#pragma once

#include "IDrawable.h"

#include "GL/glew.h"

namespace ShadingCookbook
{
class Sphere : IDrawable
{
public:
    Sphere(float radius, GLuint slices, GLuint stacks);
    /**
     * \brief Render sphere.
     */
    void Render() override;
    /**
     * \brief Get objects vertex array object handler.
     */
    int GetVAOHandle();

private:
    GLuint _vao;
    GLuint _nVerts, _elements;
    GLuint _slices, _stacks;
    float _radius;

    /**
     * \brief Generate sphere vertices.
     */
    void GenerateVerts(float* verts, float* norms, float* tex, unsigned int* el);
};
}
