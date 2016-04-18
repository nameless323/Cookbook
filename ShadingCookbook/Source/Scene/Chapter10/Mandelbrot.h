#pragma once

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"

#include <glm.hpp>
#include "../../Items/Cube.h"
using glm::mat4;

class Mandelbrot : public Scene
{
public:
    Mandelbrot();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t);
    void Render() override;
    void Shutdown();
    void Resize(int x, int y);
    void SetWindow();
private:
    ShaderProgram _shader;
    ShaderProgram _computeShader;
    int _width, _height;
    float _time, _dt, _speed, _angle, _rotSpeed;
    float _cHeight;
    GLuint _dataBuf, _fsQuad;
    Cube* _cube;
    vec2 _center;
    bool _paused;

    void SetMatrices();
    void CompileAndLinkShader();
    void InitBuffers();
};