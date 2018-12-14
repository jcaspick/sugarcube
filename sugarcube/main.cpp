#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <string>
#include <fstream>
#include <iostream>

#include "Sugarcube.h"
#include "OrthoCamera.h"
#include "PerspCamera.h"

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using ivec3 = glm::ivec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

const GLsizei SCREEN_WIDTH = 1100;
const GLsizei SCREEN_HEIGHT = 800;
const float SIDEBAR_WIDTH = 300.0f;

float lastFrame;
float elapsed;

Sugarcube sugarcube(SCREEN_WIDTH, SCREEN_HEIGHT, SIDEBAR_WIDTH);

void resizeCallback(GLFWwindow* window, int width, int height);

int main() {
	// initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window and OpenGL context
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "sugarcube", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// create viewport
	glViewport(0, 0, SCREEN_WIDTH - SIDEBAR_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	// resize callback
	glfwSetFramebufferSizeCallback(window, resizeCallback);

	// SETUP IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();

	// camera
	OrthoCamera camera(window, SCREEN_WIDTH - SIDEBAR_WIDTH, SCREEN_HEIGHT);
	sugarcube.camera = &camera;

	sugarcube.initialize();

	// event loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		camera.handleMouse();

		float currentTime = glfwGetTime();
		float dt = currentTime - lastFrame;
		lastFrame = currentTime;

		sugarcube.update(dt);
		sugarcube.draw();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void resizeCallback(GLFWwindow* window, int width, int height) {
	sugarcube.resize(width, height);
	sugarcube.camera->setSize(width - SIDEBAR_WIDTH, height);
	glViewport(0, 0, width - SIDEBAR_WIDTH, height);
}