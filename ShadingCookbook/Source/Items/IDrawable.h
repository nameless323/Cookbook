//
// Interface for drawable objects.
//

#pragma once

namespace ShadingCookbook
{
class IDrawable
{
public:
    virtual ~IDrawable()
    {
    }

    IDrawable()
    {
    }

    /**
     * \brief Render object.
     */
    virtual void Render() = 0;
};
}