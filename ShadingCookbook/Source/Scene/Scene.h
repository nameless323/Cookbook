#pragma once

namespace ShadingCookbook
{
class Scene
{
public:
    Scene() : ShouldAnimate(true)
    {
    }
    /**
     * \brief Make scene initialization (object creation, create initial matrix etc).
     */
    virtual void InitScene() = 0;
    /**
     * \brief Update scene logic.
     */
    virtual void Update(float t) = 0;
    /**
     * \brief Render scene.
     */
    virtual void Render() = 0;
    /**
     * \brief Shutdown scene. Preforms cleanup of the scene.
     */
    virtual void Shutdown() = 0;
    /**
     * \brief Handle system input.
     * \param key Key code.
     * \param action Key action (pressed etc).
     */
    virtual void ProcessInput(int key, int action) = 0;
    /**
     * \brief Call when window is resized to reconstruct matrices etc.
     */
    virtual void Resize(int w, int h) = 0;
    /**
     * \brief Animate scene or not.
     */
    void Animate(bool value)
    {
        ShouldAnimate = value;
    }
    /**
     * \brief Is scene animating.
     */
    bool IsAnimating()
    {
        return ShouldAnimate;
    }
    virtual ~Scene() = default;

protected:
    bool ShouldAnimate;
};
}
