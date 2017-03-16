#include "DisplayWindow.h"



DisplayWindow::DisplayWindow(GLFWwindow* &window, const unsigned width, const unsigned height, const char* title)
{

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(-1);
	}
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, title, NULL, NULL);
	//window = glfwCreateWindow(3200, 1800, "And there was light.", glfwGetPrimaryMonitor(), NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. ");
		glfwTerminate();
		exit(-1);
	}

	glfwSetWindowPos(window, 250, 400);

	// Define the viewport dimensions

	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(-1);
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


}


DisplayWindow::~DisplayWindow()
{
}
