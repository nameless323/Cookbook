#include "Mesh.h"

#define uint unsigned int;
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using std::ifstream;

Mesh::Mesh(const char* filename, bool reCenterMesh, bool loadUv, bool genTangents) : _recenterMesh(reCenterMesh), _loadTex(loadUv), _genTang(genTangents)
{
	LoadObj(filename);
}

void Mesh::Render()
{
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, 3 * _faces, GL_UNSIGNED_INT, (GLubyte*)NULL + 0);
	glBindVertexArray(0);
}

void Mesh::LoadObj(const char* filename)
{
	vector<vec3> points;
	vector<vec3> normals;
	vector<vec2> texCoords;
	vector<GLuint> faces;

	int nFaces = 0;
	std::ifstream objStream(filename);
	if (!objStream)
	{
		std::cerr << "unable to open obj file: " << filename << std::endl;
	}
	string line, token;
	vector<int> face;
	std::getline(objStream, line);
	while(!objStream.eof())
	{
		TrimString(line);
		if (line.length() > 0 && line.at(0) != '#')
		{
			std::istringstream lineStream(line);

			lineStream >> token;
			if (token == "v")
			{
				float x, y, z;
				lineStream >> x >> y >> z;
				points.push_back(vec3(x, y, z));
			}
			else if (token == "vt" && _loadTex)
			{
				float s, t;
				lineStream >> s >> t;
				texCoords.push_back(vec2(s, t));
			}
			else if (token == "vn")
			{
				float x, y, z;
				lineStream >> x >> y >> z;
				normals.push_back(vec3(x, y, z));
			}
			else if (token == "f")
			{
				nFaces++;

				face.clear();
				size_t slash1, slash2;
				while (lineStream.good())
				{
					string vertString;
					lineStream >> vertString;
					int pIndex = -1, nIndex = -1, tcIndex = -1;

					slash1 = vertString.find("/");
					if (slash1 == string::npos)
					{
						pIndex = atoi(vertString.c_str()) - 1;
					}
					else
					{
						slash2 = vertString.find("/", slash1 + 1);
						pIndex = atoi(vertString.substr(0, slash1).c_str()) - 1;
						if (slash2 > slash1 + 1)
						{
							tcIndex = atoi(vertString.substr(slash1 + 1, slash2).c_str()) - 1;
						}
						nIndex = atoi(vertString.substr(slash2 + 1, vertString.length()).c_str()) - 1;
					}

					if (pIndex == -1)
						std::cout << "Missing point index!!!" << std::endl;
					else
						face.push_back(pIndex);

					if (_loadTex && tcIndex != -1 && pIndex != tcIndex)
						std::cout << "Texture and point indices are not consistent" << std::endl;
					if (nIndex != -1 && nIndex != pIndex)
						std::cout << "Normal and point indices are nor consistent" << std::endl;
				}

				if (face.size() > 3) //making trig fan
				{
					int v0 = face[0];
					int v1 = face[1];
					int v2 = face[2];

					faces.push_back(v0);
					faces.push_back(v1);
					faces.push_back(v2);
					for (GLuint i = 3; i < face.size(); i++)
					{
						v1 = v2;
						v2 = face[i];
						faces.push_back(v0);
						faces.push_back(v1);
						faces.push_back(v2);
					}
				}
				else
				{
					faces.push_back(face[0]);
					faces.push_back(face[1]);
					faces.push_back(face[2]);
				}
			}
		}
		getline(objStream, line);
	}
	objStream.close();

	if (normals.size() == 0)
	{
		GenerateAverageNormals(points, normals, faces);
	}

	vector<vec4> tangents;
	if (_genTang && texCoords.size() > 0)
		GenerateTangents(points, normals, faces, texCoords, tangents);

	if (_recenterMesh)
		Center(points);

	StoreVbo(points, normals, texCoords, tangents, faces);

	std::cout << "Loaded mesh from: " << filename << std::endl;
	std::cout << " " << points.size() << " points" << std::endl;
	std::cout << " " << nFaces << " faces" << std::endl;
	std::cout << " " << faces.size() / 3 << " triangles." << std::endl;
	std::cout << " " << normals.size() << " normals" << std::endl;
	std::cout << " " << tangents.size() << " tangents " << std::endl;
	std::cout << " " << texCoords.size() << " texture coordinates." << std::endl;
}

void Mesh::TrimString(string& str)
{
	const char* witeSpace = " \t\n\r";
	size_t location;
	location = str.find_first_not_of(witeSpace);
	str.erase(0, location);
	location = str.find_last_not_of(witeSpace);
	str.erase(location + 1);
}

void Mesh::StoreVbo(const std::vector<vec3>& points, const std::vector<vec3>& normals, const std::vector<vec2>& texCoords, const std::vector<vec4>& tangents, const std::vector<GLuint>& elements)
{
	GLuint nVerts = GLuint(points.size());
	_faces = GLuint(elements.size() / 3);

	float* v = new float[3 * nVerts];
	float* n = new float[3 * nVerts];
	float* tc = nullptr;
	float* tang = nullptr;

	if (texCoords.size() > 0)
	{
		tc = new float[2 * nVerts];
		if (tangents.size() > 0)
			tang = new float[4 * nVerts];
	}

	unsigned int* el = new unsigned int[elements.size()];
	int idx = 0, tcIdx = 0, tangIdx = 0;
	for (GLuint i = 0; i < nVerts; i++)
	{
		v[idx] = points[i].x;
		v[idx + 1] = points[i].y;
		v[idx + 2] = points[i].z;

		n[idx] = normals[i].x;
		n[idx + 1] = normals[i].y;
		n[idx + 2] = normals[i].z;
		idx += 3;
		if (tc != nullptr)
		{
			tc[tcIdx] = texCoords[i].x;
			tc[tcIdx + 1] = texCoords[i].y;
			tcIdx += 2;
		}
		if (tang != nullptr)
		{
			tang[tangIdx] = tangents[i].x;
			tang[tangIdx + 1] = tangents[i].y;
			tang[tangIdx + 2] = tangents[i].z;
			tang[tangIdx + 3] = tangents[i].w;
			tangIdx += 4;
		}
	}
	for (unsigned int i = 0; i < elements.size(); i++)
		el[i] = elements[i];

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	int nBuffers = 3;
	if (tc != nullptr) nBuffers++;
	if (tang != nullptr) nBuffers++;
	GLuint elementsBuffer = nBuffers - 1;

	GLuint handle[5];
	GLuint bufIdx = 0;
	glGenBuffers(nBuffers, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[bufIdx++]);
	glBufferData(GL_ARRAY_BUFFER, (3 * nVerts)*sizeof(GLfloat), v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, handle[bufIdx++]);
	glBufferData(GL_ARRAY_BUFFER, (3 * nVerts) * sizeof(GLfloat), n, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	if (tc != nullptr)
	{
		glBindBuffer(GL_ARRAY_BUFFER, handle[bufIdx++]);
		glBufferData(GL_ARRAY_BUFFER, (2 * nVerts) * sizeof(GLfloat), tc, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(2);
	}
	if (tang != nullptr)
	{
		glBindBuffer(GL_ARRAY_BUFFER, handle[bufIdx++]);
		glBufferData(GL_ARRAY_BUFFER, (4 * nVerts) * sizeof(GLfloat), tang, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(3);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[elementsBuffer]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * _faces * sizeof(GLuint), el, GL_STATIC_DRAW);
	glBindVertexArray(0);

	delete[] v;
	delete[] n;
	if (tc != nullptr) delete[] tc;
	if (tang != nullptr) delete[] tang;
	delete[] el;
	v = nullptr;
	n = nullptr;
	tc = nullptr;
	tang = nullptr;
	el = nullptr;
}

void Mesh::GenerateAverageNormals(const std::vector<vec3>& points, std::vector<vec3>& normals, const std::vector<GLuint>& faces)
{
	for (unsigned int i = 0; i < points.size(); i++)
		normals.push_back(vec3(0.0f));

	for (unsigned int i = 0; i < faces.size(); i += 3)
	{
		const vec3& p1 = points[faces[i]];
		const vec3& p2 = points[faces[i + 1]];
		const vec3& p3 = points[faces[i + 2]];

		vec3 a = p2 - p1;
		vec3 b = p3 - p1;
		vec3 n = glm::normalize(glm::cross(a, b));

		normals[faces[i]] += n;
		normals[faces[i + 1]] += n;
		normals[faces[i + 2]] += n;
	}
	for (unsigned int i = 0; i < normals.size(); i++)
		normals[i] = glm::normalize(normals[i]);
}

void Mesh::GenerateTangents(const vector<vec3>& points, const vector<vec3>& normals, const vector<GLuint>& faces, const vector<vec2>& texCoords, vector<vec4>& tangents)
{
	vector<vec3> tan1Accum;
	vector<vec3> tan2Accum;

	for (unsigned int i = 0; i < points.size(); i++)
	{
		tan1Accum.push_back(vec3(0.0f));
		tan2Accum.push_back(vec3(0.0f));
		tangents.push_back(vec4(0.0f));
	}

	for (unsigned int i = 0; i < faces.size(); i += 3)
	{
		//http://stackoverflow.com/questions/5255806/how-to-calculate-tangent-and-binormal
		const vec3& p1 = points[faces[i]];
		const vec3& p2 = points[faces[i + 1]];
		const vec3& p3 = points[faces[i + 2]];

		const vec2& uv1 = texCoords[faces[i]];
		const vec2& uv2 = texCoords[faces[i + 1]];
		const vec2& uv3 = texCoords[faces[i + 2]];

		vec3 q1 = p2 - p1;
		vec3 q2 = p3 - p1;

		float s1 = uv2.x - uv1.x;
		float s2 = uv3.x - uv1.x;
		float t1 = uv2.y - uv1.y;
		float t2 = uv3.y - uv1.y;

		float r = 1.0f / (s1 * t2 - s2 * t1);
		vec3 tan1(
			(t2*q1.x - t1*q2.x) * r,
			(t2*q1.y - t1*q2.y) * r,
			(t2*q1.z - t1*q2.z) * r
			);
		vec3 tan2(
			(s1*q2.x - s2*q1.x) * r,
			(s1*q2.y - s2*q1.y) * r,
			(s1*q2.z - s2*q1.z) * r
			);
		tan1Accum[faces[i]] += tan1;
		tan1Accum[faces[i + 1]] += tan1;
		tan1Accum[faces[i + 2]] += tan1;
		tan2Accum[faces[i]] += tan2;
		tan2Accum[faces[i + 1]] += tan2;
		tan2Accum[faces[i + 2]] += tan2;
	}
	for (unsigned int i = 0; i < points.size(); i++)
	{
		const vec3& n = normals[i];
		vec3& t1 = tan1Accum[i];
		vec3& t2 = tan2Accum[i];
		tangents[i] = vec4(glm::normalize(t1 - (glm::dot(n, t1) * n)), 0.0f);
		tangents[i].w = (glm::dot(glm::cross(n, t1), t2) < 0.0f) ? -1.0f : 1.0f;
	}
	tan1Accum.clear();
	tan2Accum.clear();
}

void Mesh::Center(std::vector<vec3>& points)
{
	if (points.size() < 1) return;

	vec3 maxPoint = points[0];
	vec3 minPoint = points[0];

	for (unsigned int i = 0; i < points.size(); i++) //detecting aabb
	{
		vec3& point = points[i];
		if (point.x > maxPoint.x) maxPoint.x = point.x;
		if (point.y > maxPoint.y) maxPoint.y = point.y;
		if (point.z > maxPoint.z) maxPoint.z = point.z;
		if (point.x < minPoint.x) minPoint.x = point.x;
		if (point.y < minPoint.y) minPoint.y = point.y;
		if (point.z < minPoint.z) minPoint.z = point.z;
	}
	// Center of the AABB
	vec3 center = vec3((maxPoint.x + minPoint.x) / 2.0f,
		(maxPoint.y + minPoint.y) / 2.0f,
		(maxPoint.z + minPoint.z) / 2.0f);
	for (unsigned int i = 0; i < points.size(); ++i) {
		vec3 & point = points[i];
		point = point - center;
	}
}