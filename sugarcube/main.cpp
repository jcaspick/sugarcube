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

#include "Automata3D.h"
#include "OrthoCamera.h"
#include "Shader.h"
#include "ObjExporter.h"
#include "PPM_Exporter.h"

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using ivec3 = glm::ivec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

float lastFrame;
float elapsed;
bool play = false;
float animationSpeed = 2;

float randomSeed = 0;
ivec3 startSize = vec3(2);
ivec3 environmentSize = vec3(16);
int eL = 4, eU = 5, fL = 2, fU = 6;

vec4 bgColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
vec4 nearColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
vec4 farColor = vec4(0.2f, 0.0f, 0.0f, 1.0f);
float rampScale = 2.0f;
float rampOffset = 0.5f;

Shader voxelShader;
Automata3D voxels(environmentSize, 4, 5, 2, 6);
ObjExporter exporter;
PPM_Exporter imageExporter(1024, 1024);

void drawScene(Shader& shader, OrthoCamera& camera);
void drawGui(Automata3D& automata, OrthoCamera& cam);

int main() {
	// initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window and OpenGL context
	GLFWwindow* window = glfwCreateWindow(800, 600, "sugarcube", NULL, NULL);
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
	glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST);

	// SETUP IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();

	// initialize image exporter
	imageExporter.initialize();

	// shader
	voxelShader.loadFromFile("shaders/voxel1.vs", "shaders/voxel1.fs");
	voxelShader.use();

	// create voxels
	voxels.initRenderData();
	voxels.solid(startSize);

	// camera
	OrthoCamera cam(800.0f, 600.0f, window);

	// event loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// start imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		drawGui(voxels, cam);
		cam.handleMouse();

		glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float currentTime = glfwGetTime();
		elapsed += currentTime - lastFrame;
		lastFrame = currentTime;
		if (elapsed > (1.0f / animationSpeed)) {
			elapsed -= 1.0f / animationSpeed;
			if (play) voxels.update();
		}

		drawScene(voxelShader, cam);

		// draw imgui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void drawScene(Shader& shader, OrthoCamera& camera) {
	shader.setFloat("rampScale", rampScale);
	shader.setFloat("rampOffset", rampOffset);
	shader.setVec4("nearColor", nearColor);
	shader.setVec4("farColor", farColor);
	shader.setMat4("view", camera.getViewMatrix());
	shader.setMat4("projection", camera.getProjectionMatrix());
	voxels.draw();
}

void drawGui(Automata3D& automata, OrthoCamera& cam) {
	ImGui::Begin("Controls");
	ImGui::Checkbox("play", &play);
	ImGui::InputFloat("animation speed", &animationSpeed, 0.5f, 1.0f);
	if (ImGui::Button("step forward")) {
		automata.update();
	}
	ImGui::InputFloat("random seed", &randomSeed, 0.1f, 1.0f);
	ImGui::InputInt3("start size", &startSize.x, 1.0f);
	if (ImGui::Button("randomize")) {
		automata.randomize(startSize, randomSeed);
	}
	ImGui::SameLine();
	if (ImGui::Button("solid block")) {
		automata.solid(startSize);
	}

	ImGui::InputInt("EL", &eL, 1.0f, 1.0f);
	ImGui::InputInt("EU", &eU, 1.0f, 1.0f);
	ImGui::InputInt("FL", &fL, 1.0f, 1.0f);
	ImGui::InputInt("FU", &fU, 1.0f, 1.0f);
	ImGui::InputInt3("size", &environmentSize.x);

	if (ImGui::Button("new")) {
		automata = Automata3D(environmentSize, eL, eU, fL, fU);
		automata.solid(startSize);
	}

	ImGui::InputFloat("ramp scale", &rampScale, 0.1f, 1.0f);
	ImGui::InputFloat("ramp offset", &rampOffset, 0.1f, 1.0f);
	ImGui::ColorEdit4("bg color", &bgColor.r);
	ImGui::ColorEdit4("near color", &nearColor.r);
	ImGui::ColorEdit4("far color", &farColor.r);

	if (ImGui::Button("isometric")) {
		cam.setAzimuth(45);
		cam.setAltitude(35.264f);
	}

	if (ImGui::Button("export")) {
		exporter.load(automata.cells, automata.size);
		exporter.exportObj();
	}

	if (ImGui::Button("save image")) {
		imageExporter.beginCapture();
		cam.setSize(1024, 1024);
		drawScene(voxelShader, cam);
		imageExporter.saveImage();
		cam.setSize(800, 600);
	}

	ImGui::End();
}