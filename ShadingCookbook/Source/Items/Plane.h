//
// Plane generator.
//

#pragma once

#include "IDrawable.h"

namespace ShadingCookbook
{
class Plane : public IDrawable
{
public:
    Plane(float xSize, float ySize, int dDivs, int zDivs, float sMax = 1.0f, float tMax = 1.0f);
    /**
     * \brief Render plane.
     */
    void Render() override;

private:
    unsigned int _vao;
    int _faces;
};
}
