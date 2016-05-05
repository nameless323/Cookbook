#include "Frustum.h"

Frustum::Frustum(Projection::ProjType type) : _type(type)
{
    GLuint elem[12 * 2];
    this->Orient(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    if (type == Projection::ORTHO)
    {
        this->SetOrthoBounds(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    }
    else
    {
        this->SetPerspective(50.0f, 1.0f, 0.5f, 100.0f);

        GLuint idx = 0;
        elem[idx++] = 0; elem[idx++] = 5;
        elem[idx++] = 0; elem[idx++] = 6;
        elem[idx++] = 0; elem[idx++] = 7;
        elem[idx++] = 0; elem[idx++] = 8;

        elem[idx++] = 0; elem[idx++] = 2;
        elem[idx++] = 1; elem[idx++] = 3;
        elem[idx++] = 2; elem[idx++] = 4;
        elem[idx++] = 3; elem[idx++] = 1;

        elem[idx++] = 5; elem[idx++] = 6;
        elem[idx++] = 6; elem[idx++] = 7;
        elem[idx++] = 7; elem[idx++] = 8;
        elem[idx++] = 8; elem[idx++] = 5;
    }

    glBindVertexArray(0);
    glGenBuffers(2, _handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), elem, GL_STATIC_DRAW);
        
}

void Frustum::Orient(const vec3& pos, const vec3& a, const vec3& u)
{
    _origin = pos;
    _at = a;
    _up = u;
}

void Frustum::SetOrthoBounds(float xmin, float xmax, float ymin, float ymax, float near, float far)
{
    _xmin = xmin;
    _xmax = xmax;
    _ymin = ymin;
    _ymax = ymax;
    _near = near;
    _far = far;
}

void Frustum::SetPerspective(float fovy, float ar, float near, float far)
{
    _fovy = fovy;
    _ar = ar;
    _near = near;
    _far = far;
}

void Frustum::Enclose(const Frustum& other)
{
    vec3 n = glm::normalize(other._origin - other._at);
    vec3 u = glm::normalize(cross(other._up, n));
    vec3 v = glm::normalize(cross(n, u));
    if (_type == Projection::PERSPECTIVE)
        Orient(_origin, other.GetCenter(), _up);
    mat4 m = GetViewMatrix();

    vec3 p[8];
    if (other._type == Projection::PERSPECTIVE)
    {
        float dy = other._near * tanf(radians(other._fovy) / 2.0f);
        float dx = other._ar * dy;
        vec3 c = other._origin - n * other._near;
        p[0] = c + u * dx + v * dy;
        p[1] = c - u * dx + v * dy;
        p[2] = c - u * dx - v * dy;
        p[3] = c + u * dx - v * dy;
        dy = other._far * tanf(glm::radians(other._fovy) / 2.0f);
        dx = other._ar * dy;
        c = other._origin - n * other._far;
        p[4] = c + u * dx + v * dy;
        p[5] = c - u * dx + v * dy;
        p[6] = c - u * dx - v * dy;
        p[7] = c + u * dx - v * dy;
    }
    else
    {
        vec3 c = other._origin - n * other._near;
        p[0] = c + u * other._xmax + v * other._ymax;
        p[1] = c + u * other._xmax + v * other._ymin;
        p[2] = c + u * other._xmin + v * other._ymax;
        p[3] = c + u * other._xmin + v * other._ymin;
        c = other._origin - n * other._far;
        p[4] = c + u * other._xmax + v * other._ymax;
        p[5] = c + u * other._xmax + v * other._ymin;
        p[6] = c + u * other._xmin + v * other._ymax;
        p[7] = c + u * other._xmin + v * other._ymin;
    }
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