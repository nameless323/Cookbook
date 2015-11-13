#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utils/Utils.h"
#include <iostream>

#include "Source/Scene/Scene.h"

#define WIN_WIDTH 1024
#define WIN_HEIGHT 768
#include "Source/Scene/Chapter2/DiffuseScene.h"

Scene* scene;
GLFWwindow* window;

//std::string parsseCLArgs(int argc, char** argv);
//void PringHelpInfo(const char* exeFile);

void InitializeGL()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	scene->InitScene();
}

void MainLoop()
{
	while (!glfwWindowShouldClose(window) & !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		scene->Update(float(glfwGetTime()));
		scene->Render();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void ResizeGL(int w, int h)
{
	scene->Resize(w, h);
}

int main(void)
{
	scene = new DiffuseScene();

	if (!glfwInit()) 
		exit(EXIT_FAILURE);

//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	std::string title = "Blablabla";
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, title.c_str(), NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "glew failed";
	}
	GLint glMaj;
	GLint glMin;
	glGetIntegerv(GL_MAJOR_VERSION, &glMaj);
	glGetIntegerv(GL_MINOR_VERSION, &glMin);
	GLuint vao;

	InitializeGL();
	ResizeGL(WIN_WIDTH, WIN_HEIGHT);

	MainLoop();

	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}
