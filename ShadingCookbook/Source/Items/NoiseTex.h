//
// Helper for noise textures.
//

#pragma once

#include <GL/glew.h>

namespace ShadingCookbook { namespace NoiseTex
{
/**
 * \brief Store data to texture.
 */
int StoreTex(GLubyte* data, int w, int h);
/**
 * \brief Generate noise texture. 
 */
int Generate2DTex(float baseFreq = 4.0f, float persistence = 0.5f, int w = 128, int h = 128, bool periodic = false);
/**
 * \brief Generate periodic noise texture. 
 */
int GeneratePeriodic2DTex(float baseFreq = 4.0f, float persist = 0.5f, int w = 128, int h = 128);
}}
