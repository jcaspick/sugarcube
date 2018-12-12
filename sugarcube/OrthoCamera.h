#pragma once
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

using mat4 = glm::mat4;
using vec4 = glm::vec4;
using vec3 = glm::vec3;
using vec2 = glm::vec2;

class OrthoCamera {
	
public:
	OrthoCamera(GLFWwindow* window, float screenWidth, float screenHeight);
	void handleMouse();

	mat4 getViewMatrix();
	mat4 getProjectionMatrix(bool flipY = false);

	void setSize(float w, float h);
	void setAzimuth(float az);
	void setAltitude(float at);

	float zoom;
	float zoomSensitivity;
	vec3 pos;

private:
	GLFWwindow* window;
	float azimuth, altitude;
	float nearClip, farClip;
	float screenWidth, screenHeight;

	bool mouseOrbit, mouseZoom;
	glm::dvec2 clickPos;
	vec2 lastOrientation;
	float lastZoom;
};