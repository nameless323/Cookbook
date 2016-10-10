//
// Torus generator.
//

#pragma once

#include "IDrawable.h"

#include <GL/glew.h>

namespace ShadingCookbook
{
class Torus : public IDrawable
{
public:
    Torus(float outerRadius, float innerRadius, int nSides, int nRings);
    /**
     * \brief Render torus.
     */
    void Render() override;
    /**
     * \brief Get torus vertex array handle.
     */
    int GetVertexArrayHandle();

private:
    GLuint _vaoHandle;
    int _faces, _rings, _sides;

    /**
     * \brief Generate torus vertices.
     */
    void GenerateVerts(float* verts, float* norms, float* tex, unsigned int* el, float outerRadius, float innerRadius);
};
}