#pragma once

#include <GL/glew.h>
#include <stdexcept>
#include <map>
#include <glm.hpp>

using namespace glm;

namespace Projection
{
    enum ProjType
    {
        PERSPECTIVE, ORTHO
    };
};

class Frustum
{
public:
    Frustum(Projection::ProjType type);

    void Orient(const vec3& pos, const vec3& a, const vec3& u);
    void SetOrthoBounds(float xmin, float xmax, float ymin, float ymax, float near, float far);
    void SetPerspective(float fovy, float ar, float near, float far);
    void Enclose(const Frustum& other);

    mat4 GetViewMatrix() const;
    mat4 GetProjectionMatrix() const;
    vec3 GetOrigin() const;
    vec3 GetCenter() const;

    void PrintInfo() const;
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