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
#include "Source/Scene/Chapter3/SpotLight.h"
#include "Source/Scene/Chapter3/ToonScene.h"
#include "Source/Scene/Chapter3/FogScene.h"
#include "Source/Scene/Chapter4/TextureScene.h"
#include "Source/Scene/Chapter4/DualTex.h"
#include "Source/Scene/Chapter4/TexDiscard.h"
#include "Source/Scene/Chapter4/NormalMapping.h"
#include "Source/Scene/Chapter4/CubemapReflect.h"
#include "Source/Scene/Chapter4/CubemapRefract.h"
#include "Source/Scene/Chapter4/ProjectTex.h"
#include "Source/Scene/Chapter4/RenderToTex.h"
#include "Source/Scene/Chapter5/EdgeOutliner.h"
#include "Source/Scene/Chapter5/GaussianBlur.h"
#include "Source/Scene/Chapter5/Tonemapping.h"
#include "Source/Scene/Chapter5/Bloom.h"
#include "Source/Scene/Chapter5/GammaCorrection.h"
#include "Source/Scene/Chapter5/Deferred.h"
#include "Source/Scene/Chapter10/Particles.h"
#include "Source/Scene/Chapter10/Cloth.h"
#include "Source/Scene/Chapter10/EdgeOutlinerComp.h"
#include "Source/Scene/Chapter10/Mandelbrot.h"
#include "Source/Scene/Chapter5/OIT.h"
#include "Source/Scene/Chapter7/Shadowmapping.h"
#include "Source/Scene/Chapter7/JitterShadowSampling.h"
#include "Source/Scene/Chapter7/ShadowVolume.h"
#include "Source/Scene/Chapter6/PointSprite.h"
#include "Source/Scene/Chapter6/ShadedWireframe.h"
#include "Source/Scene/Chapter6/Silhouette.h"
#include "Source/Scene/Chapter6/BezierCurve.h"
#include "Source/Scene/Chapter6/QuadTess.h"
#include "Source/Scene/Chapter6/TessTeapot.h"
#include "Source/Scene/Chapter6/TeapotTessDepth.h"
#include "Source/Scene/Chapter8/Sky.h"
#include "Source/Scene/Chapter8/Wood.h"
#include "Source/Scene/Chapter8/Decay.h"
#include "Source/Scene/Chapter8/Paint.h"
#include "Source/Scene/Chapter8/NightVision.h"

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
	scene = new Paint;

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
	delete scene;
	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}