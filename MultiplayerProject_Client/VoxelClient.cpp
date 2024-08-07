#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int WIDTH = 1280;
int HEIGHT = 720;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Window", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Failde to create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental == GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Faildes to init GLEW lib" << std::endl;
		return -1;

	}
	glViewport(0, 0, WIDTH, HEIGHT);

	while (!glfwWindowShouldClose(window)){
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
		return 0;
}