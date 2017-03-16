#pragma once

#include <iostream>
#ifdef _WIN32
#include "GL/glew.h"
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class DisplayWindow
{
public:
	DisplayWindow(GLFWwindow* &window, const unsigned width, const unsigned height, const char* title);
	~DisplayWindow();

	void initFrame(glm::vec3 clear_color);


};

inline void DisplayWindow::initFrame(glm::vec3 clear_color) {
	glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_2D);
}

