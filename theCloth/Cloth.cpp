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

}

int Cloth::getId(int direction, int id) {
	if (DIRS.WEST == direction) { return id - 1; }
	else if (DIRS.NORTHWEST == direction) { return id + res_x - 1;}
	else if (DIRS.NORTH == direction) {return id + res_x;}
	else if (DIRS.NORTHEAST == direction) {return id + res_x + 1;}
	else if (DIRS.EAST == direction) {return id + 1;}
	else if (DIRS.SOUTHEAST == direction) {return id - res_x + 1;}
	else if (DIRS.SOUTH == direction) {return id - res_x;}
	else {return id - res_x - 1;}
}

glm::vec3 Cloth::getSpringForce(int direction, int id) {

	float restLength = 0;
	if (direction == DIRS.NORTH || direction == DIRS.SOUTH) restLength = restLengthZ;
	else if(direction == DIRS.WEST || direction == DIRS.EAST) restLength = restLengthX;
	else restLength = restLengthXZ;

	glm::vec3 delta = vertices[id] - vertices[getId(direction, id)];
	float deltaLength = glm::length(delta); // distance
	float diff = (deltaLength - restLength) / deltaLength;

	return delta * diff * spring_factor;
}

// Method functions
void Cloth::accumulateForces(GLFWwindow *window) {
	
	
	// F(v) = Mg + Fwind + Fairresistance - k*sum(x_current - x_rest)



	for (int v = 0; v < vertices.size(); v++) {
		
		glm::vec3 wind = glm::vec3(0, 0, 0);
		glm::vec3 spring = glm::vec3(0, 0, 0);

		std::vector<glm::vec3> spring_directions;

		if (v % res_x != 0) { // WEST 
			glm::vec3 force = getSpringForce(DIRS.WEST, v);
			spring += force;
			spring_directions.push_back(glm::normalize(vertices[v] - vertices[getId(DIRS.WEST, v)]));
		}

		if (v < vertices.size() - res_x) {// NORTH 

			if (v % res_x != 0) {
				glm::vec3 force = getSpringForce(DIRS.NORTHWEST, v);
				spring += force;
				spring_directions.push_back(glm::normalize(vertices[v] - vertices[getId(DIRS.NORTHWEST, v)]));
			}

			glm::vec3 force = getSpringForce(DIRS.NORTH, v);
			spring += force;
			spring_directions.push_back(glm::normalize(vertices[v] - vertices[getId(DIRS.NORTH, v)]));
			
			if ((v + 1) % res_x != 0) { 
				glm::vec3 force = getSpringForce(DIRS.NORTHEAST, v);
				spring += force;
				spring_directions.push_back(glm::normalize(vertices[v] - vertices[getId(DIRS.NORTHEAST, v)]));
			}
		}
	
		if((v + 1) % res_x != 0){ // EAST 
			glm::vec3 force = getSpringForce(DIRS.EAST, v);
			spring += force;
			spring_directions.push_back(glm::normalize(vertices[v] - vertices[getId(DIRS.EAST, v)]));
		}

		if (v > res_x - 1) {// SOUTH 
			
			if ((v + 1) % res_x != 0) {
				glm::vec3 force = getSpringForce(DIRS.SOUTHEAST, v);
				spring += force;
				spring_directions.push_back(glm::normalize(vertices[v] - vertices[getId(DIRS.SOUTHEAST, v)]));
			}
			
			glm::vec3 force = getSpringForce(DIRS.SOUTH, v);
			spring += force;
			spring_directions.push_back(glm::normalize(vertices[v] - vertices[getId(DIRS.SOUTH, v)]));

			if (v % res_x != 0) {
				glm::vec3 force = getSpringForce(DIRS.SOUTHWEST, v);
				spring += force;
				spring_directions.push_back(glm::normalize(vertices[v] - vertices[getId(DIRS.SOUTHWEST, v)]));
			}
			
		}

		// Calculate normals
		glm::vec3 normal = glm::vec3(0);
		for (int i = 1; i < spring_directions.size(); i++) {
			normal += glm::cross(spring_directions[i], spring_directions[i-1]);
		}
		normals[v] = glm::normalize(normal);

		// Wind
		wind.x = sin(vertices[v].x*vertices[v].y*glfwGetTime());
		wind.y = cos(vertices[v].z*glfwGetTime());
		wind.z = sin(cos(5*vertices[v].x*vertices[v].y*vertices[v].z));
		wind *= wind_factor;

		// Wind Resistance 
		glm::vec3 F_air_resistance = - air_resistance *  velocities[v] * glm::abs(glm::dot(normals[v], velocities[v]));


		// Sphere
		glm::vec3 sphere_center = glm::vec3(0, 2.0f, 0.0f);
		float sphere_radius = 5.0f;
		float sphere_friction = 0.8f;
		if (glm::length(sphere_center - vertices[v]) < sphere_radius) {
			glm::vec3 dir = glm::normalize(sphere_center - vertices[v] );
			float factor = sphere_radius - glm::length(sphere_center - vertices[v]);
			vertices[v] -= factor * dir;
			velocities[v] *= sphere_friction;
		}

		// Controls
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			playSimulation = true;
		}

		if (glfwGetKey(window, GLFW_KEY_V)) {
			
			glm::vec3 dir = glm::vec3(20, 0, 10);
			wind = dir * glm::length(normals[v] * glm::normalize(dir));
		}

		if (glfwGetKey(window, GLFW_KEY_R)) {
			for (int v = 0; v < vertices.size(); v++) {
				vertices[v] = init_positions[v];
				velocities[v] = glm::vec3(0);
				prev_positions[v] = vertices[v];
			}
		}

		if (glfwGetKey(window, GLFW_KEY_G))
			gravity *= -1.0f;

		if (playSimulation) {
			forces[v] = wind + F_air_resistance + gravity - spring;
		}


		// Pinned vertices
		if (v > vertices.size() - res_x - 1) {

			if (v % res_x == 0) {
				forces[v] = glm::vec3(0);
			}

			if ((v + 1) % res_x == 0) {
				forces[v] = glm::vec3(0);
			}
		}

		if (v < res_x) {

			if (v % res_x == 0) {
				forces[v] = glm::vec3(0);
			}

			if ((v + 1) % res_x == 0) {
				forces[v] = glm::vec3(0);
			}
		}

		
		
	}
}

void Cloth::forwardEulerIntegration(float dt) {

	for (int v = 0; v < vertices.size(); v++) {
		glm::vec3 acceleration = forces[v] * 1.0f; // mass
		velocities[v] = damping_factor*velocities[v] + acceleration * dt;
		vertices[v] = vertices[v] + velocities[v] * dt;
	}
}

void Cloth::backwardEulerIntegration(float dt) {

	for (int v = 0; v < vertices.size(); v++) {


	}
}

void Cloth::verletIntegration(float dt, int n_iterations) {
	
	for (int v = 0; v < vertices.size(); v++) {
		glm::vec3 position_prim = glm::vec3(0);
		glm::vec3 acceleration = forces[v] * 1.0f; // mass
		for (int i = 0; i < n_iterations; i++) {
			position_prim = 2.0f * vertices[v] - prev_positions[v] + acceleration * dt * dt;
			prev_positions[v] = vertices[v];
		}
		vertices[v] = position_prim;
	}
}

/* ============================= //
	UPDATE SIMULATION
// ============================= */ 
void Cloth::updateSimulation(float dt, GLFWwindow *window) {
	accumulateForces(window);
	//verletIntegration(dt, 1);
	forwardEulerIntegration(dt);

	// Send vertices to OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, &vertices[0], GL_DYNAMIC_DRAW);

	// Send normals to OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float) * 3, &normals[0], GL_DYNAMIC_DRAW);
}



void Cloth::draw(GLFWwindow *window) {
	glLineWidth(1.0);
	glPointSize(4.0);

	// Draw lines if W pressed
	if (glfwGetKey(window, GLFW_KEY_W))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_Q))
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

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
	float height = 20.0f;

	float leap_x = (float) width / (float) res_x;
	float offset_x = (float) width / 2.0f - (float) leap_x / 2.0f; // center

	float leap_z = (float) length / (float) res_z;
	float offset_z = (float) length / 2.0f - (float) leap_z / 2.0f; // center

	restLengthX = leap_x;
	restLengthZ = leap_z;
	restLengthXZ = sqrt(pow(leap_x, 2) + pow(leap_z, 2));

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	for (float z = 0; z < res_z; z += 1.0f) {
		for (float x = 0; x < res_x; x += 1.0f) {
			vertices.push_back(glm::vec3(x*leap_x - offset_x, height, z*leap_z - offset_z));
			init_positions.push_back(glm::vec3(x*leap_x - offset_x, height, z*leap_z - offset_z));

			normals.push_back(glm::vec3(0,1,0));
			uvs.push_back(glm::vec2(x / res_x, z / res_z));

			// Properties
			forces.push_back(glm::vec3(0));
			velocities.push_back(glm::vec3(0));
			prev_positions.push_back(vertices[vertices.size() - 1]);

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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, &vertices[0], GL_DYNAMIC_DRAW); // Give our vertices to OpenGL.
	glEnableVertexAttribArray(0); // 1rst attribute buffer : vertices
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(float) * 3,(void*)0);

	// Model normals
	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float) * 3, &normals[0], GL_DYNAMIC_DRAW); // Give our vertices to OpenGL.
	glEnableVertexAttribArray(1); // 2rst attribute buffer : vertices
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	// Model uv coords
	glGenBuffers(1, &UVBO);
	glBindBuffer(GL_ARRAY_BUFFER, UVBO);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float) * 2, &uvs[0], GL_STATIC_DRAW); // Give our uvs to OpenGL.
	glEnableVertexAttribArray(2); // 3rst attribute buffer : uvs
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

	// Model indices
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}