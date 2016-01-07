#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utils/Utils.h"
#include <iostream>

#include "Source/Scene/Scene.h"

#define WIN_WIDTH 1024
#define WIN_HEIGHT 768
#include "Source/Scene/Chapter2/DiffuseScene.h"
#include "Source/Scene/Chapter2/PhongScene.h"
#include "Source/Scene/Chapter2/PhongTea.h"
#include "Source/Scene/Chapter2/Flat.h"
#include "Source/Scene/Chapter2/Subroutine.h"
#include "Source/Scene/Chapter2/Discarding.h"
#include "Source/Scene/Chapter3/Multilight.h"
#include "Source/Scene/Chapter3/PixelLight.h"

Scene* scene;
GLFWwindow* window;
int currentKey;
int keyAction;

const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
void InitializeGL()
{
	glClearBufferfv(GL_COLOR, 0, bckColor);
	scene->InitScene();
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	currentKey = key;
	keyAction = action;
}

void MainLoop()
{
	while (!glfwWindowShouldClose(window) & !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		scene->ProcessInput(currentKey, keyAction);
		scene->Update(float(glfwGetTime()));
		scene->Render();		
		glfwSwapBuffers(window);
		currentKey = -1;
		keyAction = -1;
		glfwPollEvents();
	}
}

void ResizeGL(int w, int h)
{
	scene->Resize(w, h);
}

int main(void)
{
	scene = new PixelLight();

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
	glfwSetKeyCallback(window, KeyCallback);
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