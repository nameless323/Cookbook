#pragma once

#include "IDrawable.h"

#include <GL\glew.h>

namespace ShadingCookbook
{
class Cube : IDrawable
{
public:
    Cube();
    void Render() override;
private:
    GLuint _handle;
};
}
