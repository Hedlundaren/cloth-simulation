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
#include "Texture.h"

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
	Cloth cloth = Cloth(60, 60, 25, 25);

	glUseProgram(0);
	ShaderProgram standard_program("shaders/standard.vert", "", "", "", "shaders/standard.frag");
	ShaderProgram cloth_program("shaders/cloth.vert", "", "", "", "shaders/cloth.frag");

	Texture albin("textures/albin.png");
	Texture carpet("textures/carpet.png");

	GLint texLoc;

	do {
		// Init frame
		myWindow.initFrame(clear_color);
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		rotator.poll(window);

		// Simulate
		if(!glfwGetKey(window, GLFW_KEY_P))
			cloth.updateSimulation(deltaTime, window);


		// Draw
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		cloth_program();
		cloth_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, currentTime, clear_color);

		
		texLoc = glGetUniformLocation(cloth_program, "albinTexture");
		glUniform1i(texLoc, 0);
		texLoc = glGetUniformLocation(cloth_program, "carpetTexture");
		glUniform1i(texLoc, 1);

		glActiveTexture(GL_TEXTURE0);
		albin.bindTexture();
		glActiveTexture(GL_TEXTURE1);
		carpet.bindTexture();
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