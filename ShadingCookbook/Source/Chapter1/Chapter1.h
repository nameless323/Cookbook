#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../Utils/Utils.h"
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <windows.h>

const char * getTypeString(GLenum type) {
	// There are many more types than are covered here, but
	// these are the most common in these examples.
	switch (type) {
	case GL_FLOAT:
		return "float";
	case GL_FLOAT_VEC2:
		return "vec2";
	case GL_FLOAT_VEC3:
		return "vec3";
	case GL_FLOAT_VEC4:
		return "vec4";
	case GL_DOUBLE:
		return "double";
	case GL_INT:
		return "int";
	case GL_UNSIGNED_INT:
		return "unsigned int";
	case GL_BOOL:
		return "bool";
	case GL_FLOAT_MAT2:
		return "mat2";
	case GL_FLOAT_MAT3:
		return "mat3";
	case GL_FLOAT_MAT4:
		return "mat4";
	default:
		return "?";
	}
}

void APIENTRY GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::cout << "GLDEBUG" << std::endl << source << " : " << type << " : " << " : " << severity << " : " << id << " : " << message << std::endl << std::endl;
}

inline int ChapterOne(void)
{
	GLFWwindow* window;
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glewInit();

	glDebugMessageCallback(GLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	/*
	std::cout << "rnd " << renderer << std::endl;
	std::cout << "vendor " << vendor << std::endl;
	std::cout << "version " << version << std::endl;
	std::cout << "glslVersion " << glslVersion << std::endl;
	std::cout << "major " << major << std::endl;
	std::cout << "minor " << minor << std::endl;
	*/
	GLint extNum;
	glGetIntegerv(GL_NUM_EXTENSIONS, &extNum);
	//for (int i = 0; i < extNum; i++)
	//	std::cout << glGetStringi(GL_EXTENSIONS, i) <<std::endl;

	const GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertShaderSource = loadShaderAsString("Shaders/Ch1/basic.vert");
	const GLchar* vSourceArray[] = { vertShaderSource };
	glShaderSource(vertShader, 1, vSourceArray, 0);
	glCompileShader(vertShader);

	GLint result;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		std::cout << "Vertex shader compilation failed" << std::endl;

		GLint logLen;
		glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = new char[logLen];

			GLsizei written;
			glGetShaderInfoLog(vertShader, logLen, &written, log);
			std::cout << "Shader log:" << std::endl << log;
			delete[] log;
		}
	}

	const GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fragShaderSource = loadShaderAsString("Shaders/Ch1/basic.frag");
	const GLchar* fSourceArray[] = { fragShaderSource };
	glShaderSource(fragShader, 1, fSourceArray, 0);
	glCompileShader(fragShader);

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		std::cout << "Vertex shader compilation failed" << std::endl;

		GLint logLen;
		glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = new char[logLen];

			GLsizei written;
			glGetShaderInfoLog(fragShader, logLen, &written, log);
			std::cout << "Shader log:" << std::endl << log;
			delete[] log;
		}
	}

	const GLint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);
	GLint programStatus;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programStatus);
	if (programStatus == GL_FALSE)
	{
		std::cout << "Failed to link shader program" << std::endl;
		GLint logLen;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = new char[logLen];
			GLsizei written;
			glGetProgramInfoLog(shaderProgram, logLen, &written, log);
			std::cout << "Program log" << std::endl << log << std::endl;
			delete[] log;
		}
	}
	else
	{
		glUseProgram(shaderProgram);
	}

	GLint numAttribs;
	glGetProgramInterfaceiv(shaderProgram, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);
	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };
	std::cout << "Active attribs" << std::endl;
	for (int i = 0; i < numAttribs; i++)
	{
		GLint results[3];
		glGetProgramResourceiv(shaderProgram, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);

		GLint nameBufferSize = results[0] + 1;
		char* name = new char[nameBufferSize];
		glGetProgramResourceName(shaderProgram, GL_PROGRAM_INPUT, i, nameBufferSize, NULL, name);
		std::cout << results[2] << "||" << name << "||" << getTypeString(results[1]) << std::endl;
		delete[] name;
	}


	glDeleteShader(fragShader);
	glDeleteShader(vertShader);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLfloat pos[] =
	{
		0.0f, 0.7f, 0.5f,
		-0.7f, -0.7f, 0.5f,
		0.7f, -0.7f, 0.5f
	};
	GLuint buffer[2];
	glGenBuffers(2, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	const GLfloat colors[] =
	{
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	//glEnableVertexAttribArray(1);
	glBindVertexBuffer(1, buffer[1], 0, 3 * sizeof(GLfloat));

	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(1, 1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glUseProgram(shaderProgram);

	GLint numUniforms = 0;
	glGetProgramInterfaceiv(shaderProgram, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
	GLenum unifProperties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };
	std::cout << "active uniforms" << std::endl;
	for (int i = 0; i < numUniforms; i++)
	{
		GLint results[4];
		glGetProgramResourceiv(shaderProgram, GL_UNIFORM, i, 4, unifProperties, 4, NULL, results);
		if (results[3] != -1)
			continue;

		GLint nameBufferSize = results[0] + 1;
		char* name = new char[nameBufferSize];
		glGetProgramResourceName(shaderProgram, GL_UNIFORM, i, nameBufferSize, NULL, name);
		std::cout << results[2] << "|" << name << "|" << getTypeString(results[1]);
		delete[] name;

	}

	float angle = glm::radians(90.0f);

	GLuint vaoQuad;
	glGenVertexArrays(1, &vaoQuad);
	glBindVertexArray(vaoQuad);
	GLfloat quadVerts[] = {
		-0.9f, 0.9f, 0.5f,
		-0.9f, -0.9f, 0.5f,
		0.9f, 0.9f, 0.5f,

		0.9f, 0.9f, 0.5f,
		-0.9f, -0.9f, 0.5f,
		0.9f, -0.9f, 0.5f
	};
	GLuint qvertBuf;
	glGenVertexArrays(1, &qvertBuf);
	glBindBuffer(GL_ARRAY_BUFFER, qvertBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	GLfloat quadUV[] =
	{
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};
	GLuint quadUVBuffer;
	glGenBuffers(1, &quadUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadUV), quadUV, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	GLuint quadShader = glCreateProgram();
	const GLchar* quadVertShaderSource = loadShaderAsString("Shaders/Uniforms/Blob.vert");
	const GLchar* quadvSourceArray[] = { quadVertShaderSource };
	GLuint qVertSh = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(qVertSh, 1, quadvSourceArray, 0);
	glCompileShader(qVertSh);
	glGetShaderiv(qVertSh, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		std::cout << "Vertex shader compilation failed" << std::endl;

		GLint logLen;
		glGetShaderiv(qVertSh, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = new char[logLen];

			GLsizei written;
			glGetShaderInfoLog(qVertSh, logLen, &written, log);
			std::cout << "Shader log:" << std::endl << log;
			delete[] log;
		}
	}
	const GLchar* quadFragShaderSource = loadShaderAsString("Shaders/Uniforms/Blob.frag");
	const GLchar* quadfSourceArray[] = { quadFragShaderSource };
	GLuint qFragSh = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(qFragSh, 1, quadfSourceArray, 0);
	glCompileShader(qFragSh);
	glGetShaderiv(qFragSh, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		std::cout << "Vertex shader compilation failed" << std::endl;

		GLint logLen;
		glGetShaderiv(qFragSh, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = new char[logLen];

			GLsizei written;
			glGetShaderInfoLog(qFragSh, logLen, &written, log);
			std::cout << "Shader log:" << std::endl << log;
			delete[] log;
		}
	}

	glAttachShader(quadShader, qVertSh);
	glAttachShader(quadShader, qFragSh);
	glLinkProgram(quadShader);
	glDeleteShader(qVertSh);
	glDeleteShader(qFragSh);

	GLint programStatus1;
	glGetProgramiv(quadShader, GL_LINK_STATUS, &programStatus1);
	if (programStatus1 == GL_FALSE)
	{
		std::cout << "Failed to link shader program" << std::endl;
		GLint logLen;
		glGetProgramiv(quadShader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char* log = new char[logLen];
			GLsizei written;
			glGetProgramInfoLog(quadShader, logLen, &written, log);
			std::cout << "Program log" << std::endl << log << std::endl;
			delete[] log;
		}
	}

	numUniforms = 0;
	glGetProgramInterfaceiv(quadShader, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
	std::cout << "active uniforms" << std::endl;
	for (int i = 0; i < numUniforms; i++)
	{
		GLint results[4];
		glGetProgramResourceiv(quadShader, GL_UNIFORM, i, 4, unifProperties, 4, NULL, results);
		if (results[3] != -1)
			continue;

		GLint nameBufferSize = results[0] + 1;
		char* name = new char[nameBufferSize];
		glGetProgramResourceName(quadShader, GL_UNIFORM, i, nameBufferSize, NULL, name);
		std::cout << results[2] << "|" << name << "|" << getTypeString(results[1]);
		delete[] name;

	}
	GLuint blockIndex = glGetUniformBlockIndex(quadShader, "BlobSettings");
	if (blockIndex == GL_INVALID_INDEX)
		std::cout << std::endl << "invalid block index" << std::endl;
	GLint blockSize;
	glGetActiveUniformBlockiv(quadShader, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	GLubyte* blockBuffer = (GLubyte*)malloc(blockSize);
	const GLchar* uninames[] = { "innerColor", "outerColor", "radiusInner", "radiusOuter" };
	//const GLchar* uninames[] = {"innerColor", "outerColor", "radiusInner", "radiusOuter"};
	GLuint indices[4];
	glGetUniformIndices(quadShader, 4, uninames, indices);
	GLint offsets[4];
	glGetActiveUniformsiv(quadShader, 4, indices, GL_UNIFORM_OFFSET, offsets);
	GLfloat outerColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat innerColor[] = { 1.0f, 1.0f, 0.75f, 1.0f };
	GLfloat innerRadius = 0.25f;
	GLfloat outerRadius = 0.45f;
	memcpy(blockBuffer + offsets[0], innerColor, 4 * sizeof(GLfloat));
	memcpy(blockBuffer + offsets[1], outerColor, 4 * sizeof(GLfloat));
	memcpy(blockBuffer + offsets[2], &innerRadius, sizeof(GLfloat));
	memcpy(blockBuffer + offsets[3], &outerRadius, sizeof(GLfloat));
	GLuint uboBuffer;
	glGenBuffers(1, &uboBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uboBuffer);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
	free(blockBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboBuffer);
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);

		/* Render here */
		glBindVertexArray(vao);
		glUseProgram(shaderProgram);
		GLint rotMatLocation = glGetUniformLocation(shaderProgram, "rotMat");
		glm::mat4 rotMat = rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(rotMatLocation, 1, GL_FALSE, &rotMat[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(vaoQuad);
		glUseProgram(quadShader);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDeleteVertexArrays(1, &vao);
	glDeleteVertexArrays(1, &vaoQuad);
	glUseProgram(0);
	glDeleteProgram(shaderProgram);
	glDeleteProgram(quadShader);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, buffer);
	glDeleteBuffers(1, &quadUVBuffer);
	glDeleteBuffers(1, &qvertBuf);
	glDeleteBuffers(1, &uboBuffer);
	glfwTerminate();
	return 0;
}
