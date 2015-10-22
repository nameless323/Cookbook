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

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		const GLfloat bckColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		
		/* Render here */

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
