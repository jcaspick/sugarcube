#pragma once
#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Automata3D.h"
#include "ObjExporter.h"
#include "PPM_Exporter.h"

using vec2 = glm::vec2;
using ivec2 = glm::ivec2;
using vec3 = glm::vec3;
using ivec3 = glm::ivec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

enum class ShaderType {
	Ramp,
	Normal
};

class Sugarcube {

public:
	Sugarcube(float screenWidth, float screenHeight, float sidebarWidth);
	void initialize();
	void update(float dt);
	void draw();
	void resize(float width, float height);

	Camera* camera;

private:
	void drawScene(bool flipY = false);
	void drawGui();

	vec2 screen;
	float sidebarWidth;

	float elapsed;
	float playSpeed;
	bool playing;

	ShaderType shader;
	Shader rampShader;
	Shader normalShader;

	vec4 bgColor;

	int rampMode;
	int smoothLight;
	vec4 nearColor;
	vec4 farColor;
	vec4 innerColor;
	vec4 outerColor;
	float originRampScale;
	float originRampOffset;
	float cameraRampScale;
	float cameraRampOffset;

	vec4 xColor;
	vec4 yColor;
	vec4 zColor;
	vec4 lightColor;
	vec4 ambientColor;
	float lightAzimuth;
	float lightAltitude;
	float normalMix;
	float lightMix;

	Automata3D simulation;
	ObjExporter objExporter;
	PPM_Exporter imageExporter;
};