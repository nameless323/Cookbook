#include "Frustum.h"

#include <gtc/matrix_transform.inl>

namespace ShadingCookbook
{
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

    if (_type == Projection::PERSPECTIVE)
    {
        _fovy = 0.0f;
        _far = 0.0f;
        _near = std::numeric_limits<float>::max();
        float maxHorizAngle = 0.0f;
        for (int i = 0; i < 8; i++)
        {
            vec4 pt = m * vec4(p[i], 1.0f);

            if (pt.z < 0.0f)
            {
                float d = -pt.z;
                float angle = atanf(fabs(pt.x) / d);
                if (angle > maxHorizAngle)
                    maxHorizAngle = angle;
                angle = glm::degrees(atanf(fabs(pt.y) / d));
                if (angle * 2.0f > _fovy)
                    _fovy = angle * 2.0f;
                if (_near > d)
                    _near = d;
                if (_far < d)
                    _far = d;
            }
        }
        float h = (_near * tanf(glm::radians(_fovy) / 2.0f))*2.0f;
        float w = (_near * tanf(maxHorizAngle)) * 2.0f;
        _ar = w / h;
    }
    else
    {
        _xmin = _ymin = _near = std::numeric_limits<float>::max();
        _xmax = _ymax = _far = std::numeric_limits<float>::min();
        for (int i = 0; i < 8; i++)
        {
            vec4 pt = m * vec4(p[i], 1.0f);
            if (_xmin > pt.x)
                _xmin = pt.x;
            if (_xmax < pt.x)
                _xmax = pt.x;
            if (_ymin > pt.y) 
                _ymin = pt.y;
            if (_ymax < pt.y) 
                _ymax = pt.y;
            if (_near > -pt.z) 
                _near = -pt.z;
            if (_far < -pt.z) 
                _far = -pt.z;
        }
    }
}

mat4 Frustum::GetViewMatrix() const
{
    return glm::lookAt(_origin, _at, _up);
}

mat4 Frustum::GetProjectionMatrix() const
{
    if (_type == Projection::PERSPECTIVE)
        return glm::perspective(glm::radians(_fovy), _ar, _near, _far);
    return glm::ortho(_xmin, _xmax, _ymin, _ymax, _near, _far);
}

vec3 Frustum::GetOrigin() const
{
    return _origin;
}

vec3 Frustum::GetCenter() const
{
    float dist = (_near + _far) / 2.0f;
    vec3 r = glm::normalize(_at - _origin);
    return _origin + (r * dist);
}

void Frustum::PrintInfo() const
{
    if (_type == Projection::PERSPECTIVE) {
        printf("Perspective:  fovy = %f  ar = %f  near = %f  far = %f\n",
            _fovy, _ar, _near, _far);
    }
    else {
        printf("Orthographic: x(%f,%f) y(%f,%f) near = %f far = %f\n",
            _xmin, _xmax, _ymin, _ymax, _near, _far);
    }
    printf("   Origin = (%f, %f, %f)  at = (%f, %f, %f) up = (%f, %f, %f)\n",
        _origin.x, _origin.y, _origin.z, _at.x, _at.y, _at.z, _up.x, _up.y, _up.z);
}

void Frustum::Render() const
{
    if (_type != Projection::PERSPECTIVE) return;

    static float vert[9 * 3];
    static vec3 p[8];

    vert[0] = _origin.x;
    vert[1] = _origin.y;
    vert[2] = _origin.z;

    vec3 n = normalize(_origin - _at);
    vec3 u = normalize(cross(n, u));
    vec3 v = normalize(cross(n, u));

    float dy = _near * tanf(radians(_fovy) / 2.0f);
    float dx = _ar * dy;
    vec3 c = _origin - n * _near;
    p[0] = c + u * dx + v * dy;
    p[1] = c - u * dx + v * dy;
    p[2] = c - u * dx - v * dy;
    p[3] = c + u * dx - v * dy;
    dy = _far * tanf(radians(_fovy) / 2.0f);
    dx = _ar * dy;
    c = _origin - n * _far;
    p[4] = c + u * dx + v * dy;
    p[5] = c - u * dx + v * dy;
    p[6] = c - u * dx - v * dy;
    p[7] = c + u * dx - v * dy;

    int idx = 3;
    for (int i = 0; i < 8; i++)
    {
        vert[idx++] = p[i].x;
        vert[idx++] = p[i].y;
        vert[idx++] = p[i].z;
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 9 * 3 * sizeof(float), vert, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle[1]);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
}
}