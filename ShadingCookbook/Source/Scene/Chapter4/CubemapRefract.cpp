#include "CubemapRefract.h"

#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>

#include "../../Core/TGA.h"

namespace ShadingCookbook
{
using glm::vec3;

CubemapRefract::CubemapRefract() : _angle(0), _prevTime(0), _autorotate(1), _rotateLeft(0), _rotateRight(0)
{
}

void CubemapRefract::ProcessInput(int key, int action)
{
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        _autorotate = !_autorotate;

    if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        _autorotate = false;
        _rotateLeft = true;
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        _autorotate = false;
        _rotateRight = true;
    }
}

void CubemapRefract::InitScene()
{
    CompileAndLinkShader();

    glEnable(GL_DEPTH_TEST);

    _teapot = new Teapot(14, mat4(1.0f));
    _skybox = new Skybox();
    _plane = new Plane(1.0f, 1.0f, 1, 1);
    float c = 3.5f;
    _torus = new Torus(0.7f * c, 0.3f * c, 50, 50);

    _projection = mat4(1.0f);

    _angle = glm::radians(90.0f);

    LoadCubeMap("./media/texture/cubemap_night/night");
}

void CubemapRefract::LoadCubeMap(const char* baseFileName)
{
    glActiveTexture(GL_TEXTURE0);

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    const char* suffixes[] = {"posx", "negx", "posy", "negy", "posz", "negz"};
    GLuint targets[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    GLint w, h;
    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, 256, 256);

    for (int i = 0; i < 6; i++)
    {
        std::string texName = std::string(baseFileName) + "_" + suffixes[i] + ".tga";
        GLubyte* data = TGA::Read(texName.c_str(), w, h);
        glTexSubImage2D(targets[i], 0, 0, 0, w, h,
                        GL_RGBA, GL_UNSIGNED_BYTE, data);
        delete[] data;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubemapRefract::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 cameraPos = vec3(7.0f * cos(_angle), 2.0f, 7.0f * sin(_angle));
    _view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    _shader.SetUniform("WorldCameraPosition", cameraPos);

    _shader.SetUniform("DrawSkyBox", true);
    _model = mat4(1.0f);
    SetMatrices();
    _skybox->Render();
    _shader.SetUniform("DrawSkyBox", false);

    _shader.SetUniform("Material.Eta", 0.94f);
    _shader.SetUniform("Material.ReflectionFactor", 0.2f);

    _model = mat4(1.0f);
    _model *= glm::translate(vec3(0.0f, -1.0f, 0.0f));
    _model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _teapot->Render();
}

void CubemapRefract::Update(float t)
{
    float dt = t - _prevTime;
    if (_prevTime == 0.0f) dt = 0.0f;
    _prevTime = t;

    _angle += 0.25f * dt;
    if (_angle > glm::two_pi<float>())
        _angle -= glm::two_pi<float>();
}

void CubemapRefract::Shutdown()
{
    delete _skybox;
    delete _plane;
}

void CubemapRefract::SetMatrices()
{
    mat4 mv = _view * _model;
    _shader.SetUniform("ModelMatrix", _model);
    _shader.SetUniform("MVP", _projection * mv);
}

void CubemapRefract::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    _projection = glm::perspective(glm::radians(50.0f), (float)w / h, 0.3f, 100.0f);
}

void CubemapRefract::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/Cubemap/CubemapRefract/CubemapRefract.vert");
        _shader.CompileShader("Shaders/Cubemap/CubemapRefract/CubemapRefract.frag");
        _shader.Link();
        _shader.Validate();
        _shader.Use();
    }
    catch (ShaderProgramException& e)
    {
        std::cerr << e.what() << std::endl;
        //exit(EXIT_FAILURE);
    }
}
}
