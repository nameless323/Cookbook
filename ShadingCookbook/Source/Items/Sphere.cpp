#include "Sphere.h"

#include <cstdio>
#include <cmath>
#include <gtc/constants.hpp>

namespace ShadingCookbook
{
Sphere::Sphere(float radius, GLuint slices, GLuint stacks) : _radius(radius), _slices(slices), _stacks(stacks)
{
    _nVerts = (slices + 1) * (stacks + 1);
    _elements = (slices * 2 * (stacks - 1)) * 3;

    float* v = new float[3 * _nVerts];
    float* n = new float[3 * _nVerts];
    float* tex = new float[2 * _nVerts];
    unsigned int* el = new unsigned int[_elements];

    GenerateVerts(v, n, tex, el);
    unsigned int handles[4];
    glGenBuffers(4, handles);

    glBindBuffer(GL_ARRAY_BUFFER, handles[0]);
    glBufferData(GL_ARRAY_BUFFER, (3 * _nVerts) * sizeof(float), v, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handles[1]);
    glBufferData(GL_ARRAY_BUFFER, (3 * _nVerts) * sizeof(float), n, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handles[2]);
    glBufferData(GL_ARRAY_BUFFER, (2 * _nVerts) * sizeof(float), tex, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handles[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _elements * sizeof(unsigned int), el, GL_STATIC_DRAW);

    delete[] v;
    delete[] n;
    delete[] tex;
    delete[] el;

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, handles[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, handles[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, handles[2]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handles[3]);

    glBindVertexArray(0);
}

void Sphere::Render()
{
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _elements, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

int Sphere::GetVAOHandle()
{
    return _vao;
}

void Sphere::GenerateVerts(float* verts, float* norms, float* tex, unsigned* el)
{
    GLfloat theta;
    GLfloat phi;
    GLfloat thetaFac = glm::two_pi<float>() / _slices;
    GLfloat phiFac = glm::pi<float>() / _stacks;
    GLfloat nx, ny, nz, s, t;
    GLuint idx = 0, tIdx = 0;
    for (GLuint i = 0; i <= _slices; i++)
    {
        theta = i * thetaFac;
        s = (GLfloat)i / _slices;
        for (GLuint j = 0; j <= _stacks; j++)
        {
            phi = j * phiFac;
            t = (GLfloat)j / _stacks;
            nx = sinf(phi) * cosf(theta);
            ny = sinf(phi) * sinf(theta);
            nz = cosf(phi);
            verts[idx] = _radius * nx;
            verts[idx + 1] = _radius * ny;
            verts[idx + 2] = _radius * nz;

            norms[idx] = nx;
            norms[idx + 1] = ny;
            norms[idx + 2] = nz;
            idx += 3;

            tex[tIdx] = s;
            tex[tIdx + 1] = t;
            tIdx += 2;
        }
    }

    idx = 0;
    for (GLuint i = 0; i < _slices; i++)
    {
        GLuint stackStart = i * (_stacks + 1);
        GLuint nextStackStart = (i + 1) * (_stacks + 1);
        for (GLuint j = 0; j < _stacks; j++)
        {
            if (j == 0)
            {
                el[idx] = stackStart;
                el[idx + 1] = stackStart + 1;
                el[idx + 2] = nextStackStart + 1;
                idx += 3;
            }
            else if (j == (_stacks - 1))
            {
                el[idx] = stackStart + j;
                el[idx + 1] = stackStart + j + 1;
                el[idx + 2] = nextStackStart + j;
                idx += 3;
            }
            else
            {
                el[idx] = stackStart + j;
                el[idx + 1] = stackStart + j + 1;
                el[idx + 2] = nextStackStart + j + 1;
                el[idx + 3] = nextStackStart + j;
                el[idx + 4] = stackStart + j;
                el[idx + 5] = nextStackStart + j + 1;
                idx += 6;
            }
        }
    }
}
}
