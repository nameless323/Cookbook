//
// Describes frustrum i.e for shadowmapping.
//

#pragma once

#include <GL/glew.h>
#include <stdexcept>
#include <map>
#include <glm.hpp>

namespace ShadingCookbook {namespace Projection
{
enum ProjType
{
    PERSPECTIVE,
    ORTHO
};
};

using namespace glm;

class Frustum
{
public:
    Frustum(Projection::ProjType type);

    /**
     * \brief Lookat for frustum.
     */
    void Orient(const vec3& pos, const vec3& a, const vec3& u);
    /**
     * \brief Set bounds for orthographic frustum. 
     */
    void SetOrthoBounds(float xmin, float xmax, float ymin, float ymax, float near, float far);
    /**
     * \brief Set bounds for perspective frustum.
     */
    void SetPerspective(float fovy, float ar, float near, float far);
    /**
     * \brief Enclose frustum to other.
     */
    void Enclose(const Frustum& other);
    /**
     * \brief Get view matrix.
     */
    mat4 GetViewMatrix() const;
    /**
     * \brief Get projection matrix.
     */
    mat4 GetProjectionMatrix() const;
    /**
     * \brief Get frustum origin.
     */
    vec3 GetOrigin() const;
    /**
     * \brief Get frustum center.
     */
    vec3 GetCenter() const;
    /**
     * \brief Log info about frustum.
     */
    void PrintInfo() const;
    /**
     * \brief Render frustum.
     */
    void Render() const;

private:
    Projection::ProjType _type;

    vec3 _origin;
    vec3 _at;
    vec3 _up;

    float _near;
    float _far;
    float _xmin, _xmax, _ymin, _ymax;
    float _fovy, _ar;

    mat4 _view;
    mat4 _proj;
    GLuint _handle[2];
};
}
