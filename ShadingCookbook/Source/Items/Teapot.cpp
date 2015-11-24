#include "Teapot.h"

Teapot::Teapot(int grid, const mat4& lidTransform)
{
	int verts = 32 * (grid + 1) * (grid + 1);
	_faces = grid * grid * 32;
	float* v = new float[verts * 3];
	float* n = new float[verts * 3];
	float* tc = new float[verts * 2];
	unsigned int* el = new unsigned int[_faces * 6];

	glGenVertexArrays(1, &_vaoHandle);
	glBindVertexArray(_vaoHandle);

	unsigned int handle[4];
	glGenBuffers(4, handle);

	GeneratePatches(v, n, tc, el, grid);
	MoveLid(grid, v, lidTransform);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), n, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, 2 * verts * sizeof(float), tc, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _faces * 6 * sizeof(unsigned int), el, GL_STATIC_DRAW);

	delete[] v;
	delete[] n;
	delete[] tc;
	delete[] el;

	glBindVertexArray(0);
}

void Teapot::Render()
{
	glBindVertexArray(_vaoHandle);
	glDrawElements(GL_TRIANGLES, 6 * _faces, GL_UNSIGNED_INT, nullptr);
}

void Teapot::GeneratePatches(float* v, float* n, float* tc, unsigned* el, int grid)
{
	float* b = new float[4 * (grid + 1)];
	float* dB = new float[4 * (grid + 1)];

	int idx = 0, elIndex = 0, tcIndex = 0;

	ComputeBasisFunctions(b, dB, grid);
	//rim
	BuildPatchReflect(0, b, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
	//body
	BuildPatchReflect(1, b, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
	BuildPatchReflect(2, b, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
	//lid
	BuildPatchReflect(3, b, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
	BuildPatchReflect(4, b, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
	//bottom
	BuildPatchReflect(5, b, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
	//handle
	BuildPatchReflect(6, b, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
	BuildPatchReflect(7, b, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
	//spout
	BuildPatchReflect(8, b, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
	BuildPatchReflect(9, b, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);

	delete[] b;
	delete[] dB;
}

void Teapot::BuildPatchReflect(int patchNum, float* b, float* dB, float* v, float* n, float* tc, unsigned* el, int& index, int& elIndex, int& tcIndex, int grid, bool reflectX, bool reflectY)
{
	vec3 patch[4][4];
	vec3 patchRevV[4][4];
	GetPatch(patchNum, patch, false);
	GetPatch(patchNum, patchRevV, true);

	BuildPatch(patch, b, dB, v, n, tc, el, index, elIndex, tcIndex, grid, mat3(1.0f), true);
	if (reflectX)
	{
		BuildPatch(patchRevV, b, dB, v, n, tc, el, index, elIndex, tcIndex, grid,
		           mat3(vec3(-1.0f, 0.0f, 0.0f),
		                vec3(0.0f, 1.0f, 0.0f),
		                vec3(0.0f, 0.0f, 1.0f)), false);
	}
	if (reflectY)
	{
		BuildPatch(patchRevV, b, dB, v, n, tc, el, index, elIndex, tcIndex, grid,
		           mat3(vec3(1.0f, 0.0f, 0.0f),
		                vec3(0.0f, -1.0f, 0.0f),
		                vec3(0.0f, 0.0f, 1.0f)), false);
	}
	if (reflectX && reflectY)
	{
		BuildPatch(patch, b, dB, v, n, tc, el, index, elIndex, tcIndex, grid,
		           mat3(vec3(-1.0f, 0.0f, 0.0f),
		                vec3(0.0f, -1.0f, 0.0f),
		                vec3(0.0f, 0.0f, 1.0f)), true);
	}
}

void Teapot::BuildPatch(vec3 patch[][4], float* b, float* dB, float* v, float* n, float* tc, unsigned int* el, int& index, int& elIndex, int& tcIndex, int grid, mat3 reflect, bool invertNormal)
{
	int startIndex = index / 3;
	float tcFactor = 1.0f / grid;

	for (int i = 0; i <= grid; i++)
	{
		for (int j = 0; j <= grid; j++)
		{
			vec3 pt = reflect * Evaluate(i, j, b, patch);
			vec3 norm = reflect * EvaluateNormal(i, j, b, dB, patch);
			if (invertNormal)
				norm = -norm;
			v[index] = pt.x;
			v[index + 1] = pt.y;
			v[index + 2] = pt.z;

			n[index] = norm.x;
			n[index + 1] = norm.y;
			n[index + 2] = norm.z;

			tc[index] = i * tcFactor;
			tc[index + 1] = j * tcFactor;

			index += 3;
			tcIndex += 2;
		}
	}

	for( int i = 0; i < grid; i++ )
    {
        int iStart = i * (grid+1) + startIndex;
        int nextiStart = (i+1) * (grid+1) + startIndex;
        for( int j = 0; j < grid; j++)
        {
            el[elIndex] = iStart + j;
            el[elIndex+1] = nextiStart + j + 1;
            el[elIndex+2] = nextiStart + j;

            el[elIndex+3] = iStart + j;
            el[elIndex+4] = iStart + j + 1;
            el[elIndex+5] = nextiStart + j + 1;

            elIndex += 6;
        }
    }
}

void Teapot::GetPatch(int patchNum, vec3 patch[][4], bool reverseV)
{
	for (int u = 0; u < 4; u++)
	{
		for (int v = 0; v < 4; v++)
		{
			if (reverseV)
			{
				patch[u][v] = vec3
					(
						TeapotData::CpSata[TeapotData::PatchData[patchNum][u * 4 + (3 - v)]][0],
						TeapotData::CpSata[TeapotData::PatchData[patchNum][u * 4 + (3 - v)]][1],
						TeapotData::CpSata[TeapotData::PatchData[patchNum][u * 4 + (3 - v)]][2]
					);
				return;
			}
			patch[u][v] = vec3
				(
					TeapotData::CpSata[TeapotData::PatchData[patchNum][u * 4 + v]][0],
					TeapotData::CpSata[TeapotData::PatchData[patchNum][u * 4 + v]][1],
					TeapotData::CpSata[TeapotData::PatchData[patchNum][u * 4 + v]][2]
				);
		}
	}
}

void Teapot::ComputeBasisFunctions(float* b, float* dB, int grid)
{
	float inc = 1.0f / grid;
	for (int i = 0; i <= grid; i++)
	{
		float t = i * inc;
		float tSqr = t * t;
		float oneMinusT = (1.0f - t);
		float oneMinusT2 = oneMinusT * oneMinusT;

		b[i * 4] = oneMinusT * oneMinusT;
		b[i * 4 + 1] = 3.0f * oneMinusT2 * t;
		b[i * 4 + 2] = 3.0f * oneMinusT * tSqr;
		b[i * 4 + 3] = t * tSqr;

		dB[i * 4] = -3.0f * oneMinusT2;
		dB[i * 4 + 1] = -6.0f * t * oneMinusT + 3.0f * oneMinusT2;
		dB[i * 4 + 2] = -3.0f * tSqr + 6.0f * t * oneMinusT;
		dB[i * 4 + 3] = 3.0f * tSqr;
	}
}

vec3 Teapot::Evaluate(int gridU, int gridV, float* b, vec3 patch[][4])
{
	vec3 p(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			p += patch[i][j] * b[gridU * 4 + i] * b[gridV * 4 + j];
		}
	}
	return p;
}

vec3 Teapot::EvaluateNormal(int gridU, int gridV, float* b, float* dB, vec3 patch[][4])
{
	vec3 du(0.0f, 0.0f, 0.0f);
	vec3 dv(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			du += patch[i][j] * dB[gridU * 4 + i] * b[gridV * 4 + j];
			dv += patch[i][j] * b[gridU * 4 + i] * dB[gridV * 4 + j];
		}
	}
	vec3 norm = cross(du, dv);
	if (length(norm) != 0.0f)
		norm = normalize(norm);

	return norm;
}

void Teapot::MoveLid(int grid, float* v, const mat4& lidTransform)
{
	int start = 3 * 12 * (grid + 1) * (grid + 1);
	int end = 3 * 20 * (grid + 1) * (grid + 1);

	for (int i = start; i < end; i += 3)
	{
		glm::vec4 vert = glm::vec4(v[i], v[i + 1], v[i + 2], 1.0f);
		vert = lidTransform * vert;
		v[i] = vert.x;
		v[i + 1] = vert.y;
		v[i + 2] = vert.z;
	}
}
