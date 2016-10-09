#pragma once

namespace ShadingCookbook
{
class Scene
{
public:
    Scene() : ShouldAnimate(true)
    {
    }

    virtual void InitScene() = 0;
    virtual void Update(float t) = 0;
    virtual void Render() = 0;
    virtual void Shutdown() = 0;
    virtual void ProcessInput(int key, int action) = 0;

    virtual void Resize(int w, int h) = 0;

    void Animate(bool value)
    {
        ShouldAnimate = value;
    }

    bool IsAnimating()
    {
        return ShouldAnimate;
    }

    virtual ~Scene() = default;
protected:
    bool ShouldAnimate;
};
}
