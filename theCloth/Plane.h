#pragma once
#include <vector>
#include <glm/glm.hpp>

#ifdef _WIN32
#include "GL/glew.h"
#endif

#include <GLFW/glfw3.h>

class Plane
{
public:
	Plane();
	Plane(const int rx, const int rz, const int w, const int l);
	~Plane();
	void draw(GLFWwindow *window);

private:
	void create_plane();
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;

	GLuint VAO, IBO, VBO, UVBO;

	int width, length;
	const int res_x, res_z;
};

