#pragma once
#include "IDrawable.h"
#include "TeapotData.h"
#include <glm.hpp>
#include <GL/glew.h>
using glm::mat3;
using glm::mat4;
using glm::vec3;

class Teapot : public IDrawable
{
public:
	Teapot(int grid, const mat4& lidTransform);
	void Render() override;
private:
	GLuint _vaoHandle;
	unsigned int _faces;

	void GeneratePatches(float* v, float* n, float* tc, unsigned int* el, int grid);
	void BuildPatchReflect(int patchNum, float* b, float* dB, float* v, float* n, float* tc,
	                       unsigned int* el, int& index, int& elIndex, int& tcIndex, int grid, bool reflectX, bool reflectY);
	void BuildPatch(vec3 patch[][4], float* b, float* dB, float* v, float* n, float* tc,
		unsigned int *el, int& index, int& elIndex, int& tcIndex, int grid, mat3 reflect,
		bool invertNormal);
	void GetPatch(int patchNum, vec3 patch[][4], bool reverseV);
	void ComputeBasisFunctions(float* b, float* dB, int grid);
	vec3 Evaluate(int gridU, int gridV, float* b, float* dB, vec3 patch[][4]);
	vec3 EvaluateNormal(int gridU, int gridV, float* b, float* dB, vec3 patch[][4]);
	void MoveLid(int grid, float* v, const mat4& lidTransform);
};
