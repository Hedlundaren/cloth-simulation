#include <iostream>
#include <algorithm>
#include <vector>

#ifdef _WIN32
#include "GL/glew.h"
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "DisplayWindow.h"
#include "ShaderProgram.h"
#include "Rotator.h"

#include "Plane.h"
#include "Cloth.h"


#define HEIGHT 1080
#define WIDTH 1920

int main() {

	std::cout << "====== Welcome to ======\n";
	std::cout << "===== Planet Cloth =====\n";
	std::cout << " \n";

	// Our first ingredience is time
	float currentTime, deltaTime, lastTime = 0.0f;
	GLFWwindow* window = nullptr;
	DisplayWindow myWindow = DisplayWindow(window, WIDTH, HEIGHT, "Without clothes, we would all be naked.");
	glm::vec3 clear_color = glm::vec3(0.7, 0.65, 0.6);

	// Controls
	MouseRotator rotator;
	rotator.init(window);

	// Create cloth
	Cloth cloth = Cloth(10, 10, 10, 10);

	glUseProgram(0);
	ShaderProgram standard_program("shaders/standard.vert", "", "", "", "shaders/standard.frag");
	ShaderProgram cloth_program("shaders/cloth.vert", "", "", "", "shaders/cloth.frag");

	do {
		// Init frame
		myWindow.initFrame(clear_color);
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		rotator.poll(window);

		// Simulate
		cloth.updateSimulation(deltaTime);

		// Draw
		cloth_program();
		cloth_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, currentTime, clear_color);
		cloth.draw(window);

		// Finish frame
		glfwSwapInterval(0);
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDisableVertexAttribArray(0);

	return 0;
}