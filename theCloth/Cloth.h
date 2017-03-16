#pragma once
#include <vector>
#include <glm/glm.hpp>

#ifdef _WIN32
#include "GL/glew.h"
#endif

#include <GLFW/glfw3.h>

class Cloth
{
public:
	Cloth();
	Cloth(const int rx, const int rz, const int w, const int l);
	~Cloth();
	void draw(GLFWwindow *window);
	void updateSimulation(float dt);
	void accumulateForces();
	void verletIntegration(float dt);
	void eulerIntegration(float dt);


private:
	void create_plane();
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;

	// Properties
	std::vector<glm::vec3> forces;
	std::vector<glm::vec3> velocities;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> prev_pos;


	GLuint VAO, IBO, VBO, UVBO;

	int width, length;
	const int res_x, res_z;
};

