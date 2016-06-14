#include "ShadowVolume.h"
#include <iostream>
#include <cstdio>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#include "../../Core/TGA.h"

using glm::vec3;

ShadowVolume::ShadowVolume() : _width(800), _height(600), _rotSpeed(0.1f), _tPrev(0)
{
}

void ShadowVolume::ProcessInput(int key, int action)
{
}

void ShadowVolume::InitScene()
{
    CompileAndLinkShader();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClearStencil(0);

    glEnable(GL_DEPTH_TEST);
    _angle = 0.0f;

    _plane = new Plane(10.0f, 10.0f, 2, 2, 5.0f, 5.0f);
    _spot = new MeshAdj("./media/spot/spot_triangulated.obj");

    SetupFBO();

    _renderShader.Use();
    _renderShader.SetUniform("LightIntensity", vec3(0.85f));

    GLfloat verts[] =
        {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f
        };
    GLuint bufHandle;
    glGenBuffers(1, &bufHandle);
    glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW);

    glGenVertexArrays(1, &_fsQuad);
    glBindVertexArray(_fsQuad);

    glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE2);
    _spotTex = TGA::LoadTex("./media/spot/spot_texture.tga");
    _brickTex = TGA::LoadTex("./media/texture/brick1.tga");
}

void ShadowVolume::Update(float t)
{
    float dt = t - _tPrev;
    if (_tPrev == 0.0f) dt = 0.0f;
    _tPrev = t;
    _angle += dt * _rotSpeed;
    if (_angle > glm::two_pi<float>()) _angle -= glm::two_pi<float>();

    UpdateLight();
}

void ShadowVolume::Render()
{
    Pass1();
    Pass2();
    Pass3();
}

void ShadowVolume::Shutdown()
{
}

void ShadowVolume::Resize(int x, int y)
{
    glViewport(0, 0, x, y);
    _width = x;
    _height = y;
}

void ShadowVolume::SetMatrices(ShaderProgram& shader)
{
    mat4 mv = _view * _model;
    shader.SetUniform("ModelViewMatrix", mv);
    shader.SetUniform("ProjMatrix", _projection);
    shader.SetUniform("NormalNatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
}

void ShadowVolume::CompileAndLinkShader()
{
    try
    {
        _volumeShader.CompileShader("Shaders/ShadowVolume/Vol.vert");
        _volumeShader.CompileShader("Shaders/ShadowVolume/Vol.geom");
        _volumeShader.CompileShader("Shaders/ShadowVolume/Vol.frag");
        _volumeShader.Link();
        _volumeShader.Validate();

        _renderShader.CompileShader("Shaders/ShadowVolume/Render.vert");
        _renderShader.CompileShader("Shaders/ShadowVolume/Render.frag");
        _renderShader.Link();
        _renderShader.Validate();

        _compShader.CompileShader("Shaders/ShadowVolume/Comp.vert");
        _compShader.CompileShader("Shaders/ShadowVolume/Comp.frag");
        _compShader.Link();
        _compShader.Validate();
    } catch (ShaderProgramException &e)
    {
        std::cerr << e.what() << std::endl;
        
    }
}

void ShadowVolume::SetupFBO()
{
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);

    GLuint ambBuffer;
    glGenRenderbuffers(GL_RENDERBUFFER, &ambBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, ambBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, _width, _height);

    glActiveTexture(GL_TEXTURE0);
    GLuint diffSpecTex;
    glGenTextures(1, &diffSpecTex);
    glBindTexture(GL_TEXTURE_2D, diffSpecTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, _width, _height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenFramebuffers(1, &_colorDepthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _colorDepthFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ambBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, diffSpecTex, 0);
    
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is complete.\n");
    }
    else
    {
        printf("Framebuffer is not complete.\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowVolume::DrawScene(ShaderProgram& shader, bool onlyShadowCasters)
{
}

void ShadowVolume::Pass1()
{
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);

    _projection = glm::infinitePerspective(glm::radians(50.0f), (float)_width / _height, 0.5f);
    _view = glm::lookAt(vec3(5.0f, 5.0f, 5.0f), vec3(0, 2, 0), vec3(0, 1, 0));

    _renderShader.Use();
    _renderShader.SetUniform("LightPosition", _view * _lightPos);

    glBindFramebuffer(GL_FRAMEBUFFER, _colorDepthFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    DrawScene(_renderShader, false);
}

void ShadowVolume::Pass2()
{
    _volumeShader.Use();
    _volumeShader.SetUniform("LightPosition", _view * _lightPos);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, _colorDepthFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, _width - 1, _height - 1, 0, 0, _width - 1, _height - 1, GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xffff);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);

    DrawScene(_volumeShader, true);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void ShadowVolume::Pass3()
{
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    _compShader.Use();

    _model = mat4(1.0f);
    _projection = _model;
    _view = _model;
    SetMatrices(_compShader);

    glBindVertexArray(_fsQuad);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void ShadowVolume::UpdateLight()
{
    _lightPos = vec4(5.0f * vec3(cosf(_angle) * 7.5f, 1.5f, sinf(_angle)* 7.5f), 1.0);
}