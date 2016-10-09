#pragma once

#include <GL/glew.h>

namespace ShadingCookbook { namespace NoiseTex
{
int StoreTex(GLubyte* data, int w, int h);
int Generate2DTex(float baseFreq = 4.0f, float persistence = 0.5f, int w = 128, int h = 128, bool periodic = false);
int GeneratePeriodic2DTex(float baseFreq = 4.0f, float persist = 0.5f, int w = 128, int h = 128);
}}
