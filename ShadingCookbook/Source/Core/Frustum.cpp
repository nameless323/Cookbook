#include "Frustum.h"

Frustum::Frustum(Projection::ProjType type)
{
}

void Frustum::Orient(const vec3& pos, const vec3& a, const vec3& u)
{
}

void Frustum::SetOrthoBounds(float xmin, float xmax, float ymin, float ymax, float near, float far)
{
}

void Frustum::SetPerspective(float fovy, float ar, float near, float far)
{
}

void Frustum::Enclose(const Frustum& other)
{
}

mat4 Frustum::GetViewMatrix() const
{
    return _view;
}

mat4 Frustum::GetProjectionMatrix() const
{
    return _proj;
}

vec3 Frustum::GetOrigin() const
{
    return _origin;
}

vec3 Frustum::GetCenter() const
{
    return _origin;
}

void Frustum::PrintInfo() const
{
}

void Frustum::Render() const
{
}