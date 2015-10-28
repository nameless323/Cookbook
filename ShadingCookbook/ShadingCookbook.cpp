#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utils/Utils.h"
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

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

int main(void)
{
	GLFWwindow* window;
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glewInit();

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
	GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION};
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

	float angle =glm::radians(90.0f);
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		
		/* Render here */
				
		GLint rotMatLocation = glGetUniformLocation(shaderProgram, "rotMat");
		glm::mat4 rotMat = rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(rotMatLocation, 1, GL_FALSE, &rotMat[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);
	glUseProgram(0);
	glDeleteProgram(shaderProgram);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, buffer);
	
	glfwTerminate();
	return 0;
}
