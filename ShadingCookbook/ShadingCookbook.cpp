#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utils/Utils.h"
#include <iostream>

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

	std::cout << "rnd " << renderer << std::endl;
	std::cout << "vendor " << vendor << std::endl;
	std::cout << "version " << version << std::endl;
	std::cout << "glslVersion " << glslVersion << std::endl;
	std::cout << "major " << major << std::endl;
	std::cout << "minor " << minor << std::endl;

	GLint extNum;
	glGetIntegerv(GL_NUM_EXTENSIONS, &extNum);
	for (int i = 0; i < extNum; i++)
		std::cout << glGetStringi(GL_EXTENSIONS, i) <<std::endl;

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
	/* Swap front and back buffers */

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glUseProgram(shaderProgram);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		
		/* Render here */
				

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
