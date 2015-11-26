#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utils/Utils.h"
#include <iostream>

#include "Source/Scene/Scene.h"

#define WIN_WIDTH 1024
#define WIN_HEIGHT 768
#include "Source/Scene/Chapter2/DiffuseScene.h"
#include "Source/Scene/Chapter2/PhongScene.h"

Scene* scene;
GLFWwindow* window;

const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
void InitializeGL()
{
	glClearBufferfv(GL_COLOR, 0, bckColor);
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
	scene = new PhongScene();

	if (!glfwInit()) exit(EXIT_FAILURE);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	std::string titile = "Blablabla";
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, titile.c_str(), nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glewInit();

	InitializeGL();
	ResizeGL(WIN_WIDTH, WIN_HEIGHT);

	MainLoop();
	scene->Shutdown();
	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}