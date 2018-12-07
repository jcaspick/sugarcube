#include "Sugarcube.h"

Sugarcube::Sugarcube(GLFWwindow* window, float screenWidth, float screenHeight) :
	window(window),
	simulation(ivec3(16), 4, 5, 2, 6),
	camera(window, screenWidth, screenHeight),
	imageExporter(1024, 1024),
	elapsed(0),
	playSpeed(2.5f),
	bgColor(0)
{}

void Sugarcube::update(float dt) {
	// time step
	elapsed += dt;
	if (elapsed > 1.0f / playSpeed) {
		simulation.step();
	}

	// handle input
	camera.handleMouse();
}

void Sugarcube::draw() {
	glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawScene();
	drawGui();
}

void Sugarcube::drawScene() {
	voxelShader.setFloat("rampScale", cameraRampScale);
	voxelShader.setFloat("rampOffset", cameraRampOffset);
	voxelShader.setVec4("nearColor", nearColor);
	voxelShader.setVec4("farColor", farColor);

	voxelShader.setMat4("view", camera.getViewMatrix());
	voxelShader.setMat4("projection", camera.getProjectionMatrix());
	simulation.draw();
}

void Sugarcube::drawGui() {
	// start imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// draw gui

	// end imgui frame
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}