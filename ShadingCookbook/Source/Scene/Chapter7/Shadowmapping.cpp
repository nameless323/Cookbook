#include "Shadowmapping.h"

#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>

#include "../../Core/TGA.h"

namespace ShadingCookbook
{
using glm::vec3;
using std::vector;

Shadowmapping::Shadowmapping() :
    _width(800), _height(600), _angle(0.0f), _tPrev(0.0f), _shadowmapWidth(512), _shadowmapHeight(512)
{
}

void Shadowmapping::ProcessInput(int key, int action)
{
    //if (key == GLFW_KEY_P && action == GLFW_PRESS)
    //    _paused = !_paused;
}

void Shadowmapping::InitScene()
{
    CompileAndLinkShader();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    _angle = glm::quarter_pi<float>();
    _teapot = new Teapot(14, mat4(1.0f));
    _plane = new Plane(40.0f, 40.0f, 2, 2);
    float scale = 2.0f;
    _torus = new Torus(0.7f * scale, 0.3f * scale, 50, 50);
    SetupFBO();

    GLuint shaderHandle = _shader.GetHandle();
    _pass1Index = glGetSubroutineIndex(shaderHandle, GL_FRAGMENT_SHADER, "recordDepth");
    _pass2Index = glGetSubroutineIndex(shaderHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

    _shadowBias = mat4(
        vec4(0.5f, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, 0.5f, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 0.5f, 0.0f),
        vec4(0.5f, 0.5f, 0.5f, 1.0f)
    );
    _lightFrustum = new Frustum(Projection::PERSPECTIVE);
    float c = 1.65f;
    vec3 lightPos = vec3(0.0f, c * 5.25f, c * 7.5f);
    _lightFrustum->Orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    _lightFrustum->SetPerspective(50.0f, 1.0f, 1.0f, 25.0f);
    _lightPV = _shadowBias * _lightFrustum->GetProjectionMatrix() * _lightFrustum->GetViewMatrix();

    _shader.SetUniform("Light.Intensity", vec3(0.85f, 0.85f, 0.85f));
    _shader.SetUniform("ShadowMap", 0);
}

void Shadowmapping::Render()
{
    _shader.Use();
    _view = _lightFrustum->GetViewMatrix();
    _projection = _lightFrustum->GetProjectionMatrix();
    glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, _shadowmapWidth, _shadowmapHeight);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_pass1Index);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    DrawScene();
    glFlush();
    glFinish();

    float c = 1.0f;
    vec3 cameraPos(c * 11.5f * cos(_angle), c * 7.0f, c * 11.5f * sin(_angle));
    _view = glm::lookAt(cameraPos, vec3(0.0), vec3(0.0f, 1.0f, 0.0f));
    _shader.SetUniform("Light.Position", _view * vec4(_lightFrustum->GetOrigin(), 1.0f));
    _projection = glm::perspective(glm::radians(50.0f), (float)_width / _height, 0.1f, 100.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, _width, _height);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_pass2Index);
    //    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    DrawScene();
}

void Shadowmapping::Update(float t)
{
    float dt = t - _tPrev;
    if (_tPrev == 0.0f)
        dt = 0.0f;
    _tPrev = t;
    _angle += 0.2f * dt;
    if (_angle > glm::two_pi<float>())
        _angle -= glm::two_pi<float>();
}

void Shadowmapping::Shutdown()
{
    delete _teapot;
    delete _plane;
    delete _lightFrustum;
    delete _torus;
}

void Shadowmapping::SetupFBO()
{
    GLfloat border[] = {1.0f, 0.0f, 0.0f, 0.0f};
    GLuint depthTex;
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, _shadowmapWidth, _shadowmapHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    glGenFramebuffers(1, &_shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

    GLenum drawBuffers[] = {GL_NONE};
    glDrawBuffers(1, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is complete. \n");
    }
    else
    {
        printf("Framebuffer is not complete. \n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadowmapping::SetMatrices()
{
    mat4 mv = _view * _model;
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection * mv);
    _shader.SetUniform("ShadowMatrix", _lightPV * _model);
}

void Shadowmapping::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    _projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void Shadowmapping::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/PercentageCloserFiltering/PercentageCloserFiltering.vert");
        _shader.CompileShader("Shaders/PercentageCloserFiltering/PercentageCloserFiltering.frag");
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

void Shadowmapping::DrawScene()
{
    vec3 color = vec3(0.7f, 0.5f, 0.3f);
    _shader.SetUniform("Material.Ka", color * 0.05f);
    _shader.SetUniform("Material.Kd", color);
    _shader.SetUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
    _shader.SetUniform("Material.Shininess", 150.0f);
    _model = mat4(1.0f);
    _model *= glm::translate(vec3(0.0f, 0.0f, 0.0f));
    _model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _teapot->Render();

    _shader.SetUniform("Material.Ka", color * 0.05f);
    _shader.SetUniform("Material.Kd", color);
    _shader.SetUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
    _shader.SetUniform("Material.Shininess", 150.0f);
    _model = mat4(1.0f);
    _model *= glm::translate(vec3(0.0f, 2.0f, 5.0f));
    _model *= glm::rotate(glm::radians(-45.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _torus->Render();

    _shader.SetUniform("Material.Kd", 0.25f, 0.25f, 0.25f);
    _shader.SetUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    _shader.SetUniform("Material.Ka", 0.05f, 0.05f, 0.05f);
    _shader.SetUniform("Material.Shininess", 1.0f);
    _model = mat4(1.0f);
    _model *= glm::translate(vec3(0.0f, 0.0f, 0.0f));
    SetMatrices();
    _plane->Render();
    _model = mat4(1.0f);
    _model *= glm::translate(vec3(-5.0f, 5.0f, 0.0f));
    _model *= glm::rotate(glm::radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
    SetMatrices();
    _plane->Render();
    _model = mat4(1.0f);
    _model *= glm::translate(vec3(0.0f, 5.0f, -5.0f));
    _model *= glm::rotate(glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _plane->Render();
    _model = mat4(1.0f);
}

void Shadowmapping::SpitOutDepthBuffer()
{
    int size = _shadowmapWidth * _shadowmapHeight;
    float* buffer = new float[size];
    unsigned char* imgBuffer = new unsigned char[size * 4];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, buffer);

    for (int i = 0; i < _shadowmapHeight; i++)
    {
        for (int j = 0; j < _shadowmapWidth; j++)
        {
            int imgIdx = 4 * ((i * _shadowmapWidth) + j);
            int bufIdx = ((_shadowmapHeight - i - 1) * _shadowmapWidth) + j;
            float minVal = 0.88f;
            float scale = (buffer[bufIdx] - minVal) / (1.0f - minVal);
            unsigned char val = (unsigned char)(scale * 255);
            imgBuffer[imgIdx] = val;
            imgBuffer[imgIdx + 1] = val;
            imgBuffer[imgIdx + 2] = val;
            imgBuffer[imgIdx + 3] = 0xff;
        }
    }
    delete[] buffer;
    delete[] imgBuffer;
    exit(1);
}
}
