#include "Sugarcube.h"
#include <iostream>
#include <nativefiledialog\nfd.h>

Sugarcube::Sugarcube(float screenWidth, float screenHeight, float sidebarWidth) :
	screen(screenWidth, screenHeight),
	sidebarWidth(sidebarWidth),
	simulation(ivec3(16), 4, 5, 2, 6),
	camera(nullptr),
	imageExporter(1024, 1024),
	elapsed(0),
	playSpeed(2.5f),
	bgColor(vec4(0.0f)),
	shader(ShaderType::Ramp),
	rampMode(0),
	smoothLight(1),
	cameraRampScale(0.5f),
	cameraRampOffset(0.5f),
	originRampScale(13.86f),
	originRampOffset(0.0f),
	nearColor(vec4(1.0f)),
	farColor(vec4(0.0f)),
	innerColor(vec4(0.5f)),
	outerColor(vec4(1.0f)),
	xColor(vec4(1, 0, 0, 1)),
	yColor(vec4(0, 1, 0, 1)),
	zColor(vec4(0, 0, 1, 1)),
	lightColor(vec4(1, 1, 1, 1)),
	normalMix(1.0f),
	lightMix(0.0f),
	playing(false)
{}

void Sugarcube::initialize() {
	// load shaders
	normalShader.loadFromFile("shaders/voxel.vs", "shaders/normal.fs");
	rampShader.loadFromFile("shaders/voxel.vs", "shaders/ramp.fs");
	rampShader.use();

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

void Sugarcube::drawScene(bool flipY) {
	// set ramp shader uniforms
	if (shader == ShaderType::Ramp) {
		rampShader.setVec4("nearColor", nearColor);
		rampShader.setVec4("farColor", farColor);
		rampShader.setVec4("innerColor", innerColor);
		rampShader.setVec4("outerColor", outerColor);
		rampShader.setFloat("cameraRampScale", cameraRampScale);
		rampShader.setFloat("cameraRampOffset", cameraRampOffset);
		rampShader.setFloat("originRampScale", originRampScale);
		rampShader.setFloat("originRampOffset", originRampOffset);
		rampShader.setFloat("rampMode", static_cast<float>(rampMode));
		rampShader.setInt("smoothLight", smoothLight);
	}

	// set normal shader uniforms
	if (shader == ShaderType::Normal) {
		normalShader.setVec4("xColor", xColor);
		normalShader.setVec4("yColor", yColor);
		normalShader.setVec4("zColor", zColor);
		normalShader.setVec4("lightColor", lightColor);
		normalShader.setFloat("normalMix", normalMix);
		normalShader.setFloat("lightMix", lightMix);
	}
	
	// set common uniforms
	rampShader.setMat4("view", camera->getViewMatrix());
	rampShader.setMat4("projection", camera->getProjectionMatrix(flipY));
	normalShader.setMat4("view", camera->getViewMatrix());
	normalShader.setMat4("projection", camera->getProjectionMatrix(flipY));

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

			if (ImGui::Combo("Shader##type", (int*)&shader, "Distance Ramp\0Normal / Light")) {
				if (shader == ShaderType::Ramp) rampShader.use();
				else if (shader == ShaderType::Normal) normalShader.use();
			}

			// ramp shader settings
			if (shader == ShaderType::Ramp) {
				ImGui::Combo("Ramp Type", &rampMode, "Distance from origin\0Distance from camera");
				ImGui::Combo("Ramp Mode", &smoothLight, "Block distance\0Vertex distance");

				if (rampMode == 0) {
					ImGui::Text("Origin Ramp");
					ImGui::ColorEdit3("Inner Color", &innerColor.r);
					ImGui::ColorEdit3("Outer Color", &outerColor.r);
					ImGui::SliderFloat("Scale##originRamp", &originRampScale, 0.0f, 50.0f);
					ImGui::SliderFloat("Offset##originRamp", &originRampOffset, -5.0f, 5.0f);
				}
				if (rampMode == 1) {
					ImGui::Text("Camera Distance Ramp");
					ImGui::ColorEdit3("Near Color", &nearColor.r);
					ImGui::ColorEdit3("Far Color", &farColor.r);
					ImGui::SliderFloat("Scale##cameraRamp", &cameraRampScale, 0.0f, 5.0f);
					ImGui::SliderFloat("Offset##cameraRamp", &cameraRampOffset, -5.0f, 5.0f);
				}
			}
			// normal shader settings
			if (shader == ShaderType::Normal) {
				ImGui::ColorEdit3("X color", &xColor.r);
				ImGui::ColorEdit3("Y color", &yColor.r);
				ImGui::ColorEdit3("Z color", &zColor.r);
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
				char* savePath = NULL;
				nfdresult_t result = NFD_SaveDialog("PPM", NULL, &savePath);

				if (result == NFD_OKAY) {
					imageExporter.beginCapture(imageSize.x, imageSize.y);
					camera->setSize(imageSize.x, imageSize.y);
					drawScene(true);
					imageExporter.saveImage(savePath);
					camera->setSize(screen.x - sidebarWidth, screen.y);
				}
				else if (result != NFD_CANCEL) {
					std::cout << "Error (nfd): " << NFD_GetError() << std::endl;
				}
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