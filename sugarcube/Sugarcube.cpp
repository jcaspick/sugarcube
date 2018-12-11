#include "Sugarcube.h"
#include <iostream>

Sugarcube::Sugarcube(float screenWidth, float screenHeight, float sidebarWidth) :
	screen(screenWidth, screenHeight),
	sidebarWidth(sidebarWidth),
	simulation(ivec3(16), 4, 5, 2, 6),
	camera(nullptr),
	imageExporter(1024, 1024),
	elapsed(0),
	playSpeed(2.5f),
	bgColor(vec4(0.0f)),
	shaderMode(0),
	smoothLight(1),
	cameraRampScale(0.5f),
	cameraRampOffset(0.5f),
	originRampScale(13.86f),
	originRampOffset(0.0f),
	nearColor(vec4(1.0f)),
	farColor(vec4(0.0f)),
	innerColor(vec4(0.5f)),
	outerColor(vec4(1.0f)),
	playing(false)
{}

void Sugarcube::initialize() {
	// load shader
	voxelShader.loadFromFile("shaders/voxel1.vs", "shaders/voxel1.fs");
	voxelShader.use();

	// initialize simulation
	simulation.initRenderData();
	simulation.createBox(ivec3(6));

	// initialize image exporter
	imageExporter.initialize();
}

void Sugarcube::update(float dt) {
	elapsed += dt;
	if (elapsed > 1.0f / playSpeed) {
		elapsed -= (1.0f / playSpeed);
		if (playing) simulation.step();
	}
}

void Sugarcube::draw() {
	glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawScene();
	drawGui();
}

void Sugarcube::resize(float width, float height) {
	screen = { width, height };
}

void Sugarcube::drawScene() {
	voxelShader.setVec4("nearColor", nearColor);
	voxelShader.setVec4("farColor", farColor);
	voxelShader.setVec4("innerColor", innerColor);
	voxelShader.setVec4("outerColor", outerColor);
	voxelShader.setFloat("cameraRampScale", cameraRampScale);
	voxelShader.setFloat("cameraRampOffset", cameraRampOffset);
	voxelShader.setFloat("originRampScale", originRampScale);
	voxelShader.setFloat("originRampOffset", originRampOffset);
	voxelShader.setFloat("rampMode", static_cast<float>(shaderMode));
	voxelShader.setInt("smoothLight", smoothLight);
	
	voxelShader.setMat4("view", camera->getViewMatrix());
	voxelShader.setMat4("projection", camera->getProjectionMatrix());

	simulation.draw();
}

void Sugarcube::drawGui() {
	// start imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	bool showOverlay = true;
	bool showSidebar = true;

	// current frame information and play/step controls
	ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin("Info", &showOverlay, ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
		ImGui::Text("Generation: %d", simulation.getGeneration());
		ImGui::Text("Rule: %d/%d/%d/%d", simulation.eL, simulation.eU, 
			simulation.fL, simulation.fU);
		ImGui::Text("Size: %dx%dx%d", simulation.getSize().x, 
			simulation.getSize().y, simulation.getSize().z);
		if (ImGui::Button(playing ? "Pause" : "Play")) {
			playing = !playing;
		}
		ImGui::SameLine();
		if (ImGui::Button("Step")) simulation.step();
	}
	ImGui::End();

	// sidebar
	ImGui::SetNextWindowPos(ImVec2(screen.x - sidebarWidth, 0), 
		ImGuiCond_Always, ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImVec2(sidebarWidth, screen.y));
	if (ImGui::Begin("Controls", &showSidebar, 
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
	{
		// simulation tab
		if (ImGui::CollapsingHeader("Simulation")) {
			ImGui::Text("Starting Shape");
			static int startShape = 0;
			ImGui::RadioButton("Box", &startShape, 0);
			ImGui::RadioButton("Cross", &startShape, 1);
			ImGui::RadioButton("Corners", &startShape, 2);
			ImGui::RadioButton("Noise", &startShape, 3);

			ImGui::Text("Shape Parameters");
			static ivec3 boxSize = ivec3(2);
			static int crossThickness = 2;
			static bool omitX = false;
			static bool omitY = false;
			static bool omitZ = false;
			static int cornerThickness = 2;
			static ivec3 noiseAreaSize = ivec3(8);

			if (startShape == 0) {
				ImGui::InputInt3("Box Size", &boxSize.x);
			}
			else if (startShape == 1) {
				ImGui::InputInt("Cross Thickness", &crossThickness);
				ImGui::Checkbox("Omit X", &omitX);
				ImGui::Checkbox("Omit Y", &omitY);
				ImGui::Checkbox("Omit Z", &omitZ);
			}
			else if (startShape == 2) {
				ImGui::InputInt("Corner Thickness", &cornerThickness);
			}
			else if (startShape == 3) {
				ImGui::InputInt3("Area Size", &noiseAreaSize.x);
			}

			static int eL = 4;
			static int eU = 5;
			static int fL = 2;
			static int fU = 6;

			ImGui::Text("Common Parameters");
			static ivec3 simulationSize = ivec3(16);
			ImGui::InputInt3("Simulation Size", &simulationSize.x);
			ImGui::SliderInt("eL", &eL, 0, 26);
			ImGui::SliderInt("eU", &eU, 0, 26);
			ImGui::SliderInt("fL", &fL, 0, 26);
			ImGui::SliderInt("fU", &fU, 0, 26);

			if (ImGui::Button("Generate", ImVec2(ImGui::GetContentRegionAvailWidth(), 30))) {
				simulation.resize(simulationSize);
				originRampScale = glm::length(static_cast<vec3>(simulationSize)) * 0.5f;
				simulation.eL = eL;
				simulation.eU = eU;
				simulation.fL = fL;
				simulation.fU = fU;
				if (startShape == 0) simulation.createBox(boxSize);
				if (startShape == 1) simulation.createCross(crossThickness, omitX, omitY, omitZ);
				if (startShape == 2) simulation.createCorners(cornerThickness);
				if (startShape == 3) simulation.createNoise(noiseAreaSize);
			}
		}

		// shader tab
		if (ImGui::CollapsingHeader("Shader")) {
			ImGui::ColorEdit3("BG Color", &bgColor.r);

			ImGui::Combo("Shader Mode", &shaderMode, "Distance from origin\0Distance from camera");
			ImGui::Combo("Ramp Mode", &smoothLight, "Block distance\0Vertex distance");

			if (shaderMode == 0) {
				ImGui::Text("Origin Ramp");
				ImGui::ColorEdit3("Inner Color", &innerColor.r);
				ImGui::ColorEdit3("Outer Color", &outerColor.r);
				ImGui::SliderFloat("Scale##originRamp", &originRampScale, 0.0f, 50.0f);
				ImGui::SliderFloat("Offset##originRamp", &originRampOffset, -5.0f, 5.0f);
			}
			if (shaderMode == 1) {
				ImGui::Text("Camera Distance Ramp");
				ImGui::ColorEdit3("Near Color", &nearColor.r);
				ImGui::ColorEdit3("Far Color", &farColor.r);
				ImGui::SliderFloat("Scale##cameraRamp", &cameraRampScale, 0.0f, 5.0f);
				ImGui::SliderFloat("Offset##cameraRamp", &cameraRampOffset, -5.0f, 5.0f);
			}
		}

		// camera tab
		if (ImGui::CollapsingHeader("Camera")) {
			if (ImGui::Button("Isometric View")) {
				camera->setAzimuth(45);
				camera->setAltitude(35.264);
			}
		}

		// export tab
		if (ImGui::CollapsingHeader("Export")) {
			static ivec2 imageSize(1024, 1024);

			ImGui::InputInt2("Image Size", &imageSize.x);

			if (ImGui::Button("Export Image")) {
				imageExporter.beginCapture(imageSize.x, imageSize.y);
				camera->setSize(imageSize.x, imageSize.y);
				drawScene();
				imageExporter.saveImage();
				camera->setSize(screen.x - sidebarWidth, screen.y);
			}
			if (ImGui::Button("Export OBJ")) {
				objExporter.load(simulation.cells, simulation.getSize());
				objExporter.exportObj();
			}
		}
	}
	ImGui::End();

	// end imgui frame
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}