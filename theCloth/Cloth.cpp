#include "Cloth.h"
#include <iostream>


Cloth::Cloth() : res_x(10), res_z(10), width(100), length(100)
{
	float resolution = 200.0f; // resolution = 10 --> 10x10
	create_plane();
}

Cloth::Cloth(const int rx, const int rz, int w, int l) : res_x(rx), res_z(rz), width(w), length(l)
{
	create_plane();
}

Cloth::~Cloth()
{
	//glDeleteBuffers(indices.size()​, IBO​);
	//glDeleteBuffers(vertices.size()​, VBO);
	//glDeleteBuffers(uvs.size()​, UVBO);
}


// Method functions
void Cloth::accumulateForces() {
	
	
	// F(v) = Mg + Fwind + Fairresistance - k*sum(x_current - x_rest)


	glm::vec3 gravity = glm::vec3(0, -9.82f, 0); 
	glm::vec3 wind = glm::vec3(0, 0, 0);
	for (int v = 0; v < vertices.size(); v++) {

		forces[v] = gravity;
		if (v % res_x == 0) { // west vertex
			forces[v] = glm::vec3(0);
		}
		else {

		}

		if (v > vertices.size() - res_x - 1) {// north vertex
			forces[v] = glm::vec3(0);
		}
		else {

		}

		if((v + 1) % res_x == 0){ // east vertex
			forces[v] = glm::vec3(0);
		}
		else {

		}
		
		if (v < res_x) {// south vertex
			forces[v] = glm::vec3(0);
		}
		else {

		}

		//forces[v] = gravity + wind;
	}
}

void Cloth::eulerIntegration(float dt) {

	for (int v = 0; v < vertices.size(); v++) {
		glm::vec3 acceleration = forces[v] * 1.0f; // mass
		velocities[v] = velocities[v] + acceleration * dt;
		positions[v] = positions[v] + velocities[v] * dt;
		vertices[v] = positions[v]; // Update vertices
	}

	//std::cout << "positions[1]: (" << positions[1].x << ", " << positions[1].y << ", " << positions[1].z << ")\n";
	//std::cout << "vertices[1]: (" << vertices[1].x << ", " << vertices[1].y << ", " << vertices[1].z  << ")\n";

	// Send vertices to OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, &vertices[0], GL_STATIC_DRAW); 
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, sizeof(float) * 3,(void*)0);
}

void Cloth::verletIntegration(float dt) {
	
	for (int v = 0; v < vertices.size(); v++) {
		// x_prim = 2x - x_prev + a * dt^2
		// x_prev = x 
	}
}

void Cloth::updateSimulation(float dt) {
	accumulateForces();
	eulerIntegration(dt);
}



void Cloth::draw(GLFWwindow *window) {
	glLineWidth(2.0);
	glPointSize(2.0);

	// Draw lines if W pressed
	if (glfwGetKey(window, GLFW_KEY_W))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

	//Draw Object
	glEnableClientState(GL_VERTEX_ARRAY);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBindVertexArray(VAO);

	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	glBindVertexArray(0);

}

void Cloth::create_plane() {
	float height = 0.0f;

	float leap_x = width / res_x;
	float offset_x = width / 2.0f - leap_x / 2.0f; // center

	float leap_z = length / res_z;
	float offset_z = length / 2.0f - leap_z / 2.0f; // center


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	for (float z = 0; z < res_z; z += 1.0f) {
		for (float x = 0; x < res_x; x += 1.0f) {
			vertices.push_back(glm::vec3(x*leap_x - offset_x, height, z*leap_z - offset_z));
			uvs.push_back(glm::vec2(x / res_x, z / res_z));

			// Properties
			forces.push_back(glm::vec3(0));
			velocities.push_back(glm::vec3(0));
			positions.push_back(vertices[vertices.size() - 1]);
			prev_pos.push_back(vertices[vertices.size() - 1]);

		}
	}

	for (int i = 0; i < res_x - 1; i++) {
		for (int j = 0; j < res_z - 1; j++) {

			// Triangle 1
			indices.push_back(i + j*res_x);
			indices.push_back((i + 1) + j*res_x);
			indices.push_back(i + (j + 1)*res_x);

			// Triangle 2
			indices.push_back((i + 1) + j*res_x);
			indices.push_back((i + 1) + (j + 1)*res_x);
			indices.push_back((i)+(j + 1)*res_x);

		}
	}

	// Model vertices
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, &vertices[0], GL_STATIC_DRAW); // Give our vertices to OpenGL.
	glEnableVertexAttribArray(0); // 1rst attribute buffer : vertices
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(float) * 3,  // stride
		(void*)0			// array buffer offset
	);

	// Model uv coords
	glGenBuffers(1, &UVBO);
	glBindBuffer(GL_ARRAY_BUFFER, UVBO);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float) * 2, &uvs[0], GL_STATIC_DRAW); // Give our uvs to OpenGL.
	glEnableVertexAttribArray(1); // 2rst attribute buffer : uvs
	glVertexAttribPointer(
		1,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(float) * 2,  // stride
		(void*)0			// array buffer offset
	);

	// Model indices
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}