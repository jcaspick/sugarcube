#include "OrthoCamera.h"
#include <iostream>
#include <cmath>

OrthoCamera::OrthoCamera(float screenWidth, float screenHeight, GLFWwindow* window) :
	window(window),
	screenWidth(screenWidth),
	screenHeight(screenHeight),
	zoom(0.03f),
	pos(vec3(0)),
	azimuth(45),
	altitude(35.264f),
	nearClip(-45),
	farClip(45),
	mouseOrbit(false),
	mouseZoom(false),
	zoomSensitivity(0.001f)
{}

mat4 OrthoCamera::getViewMatrix() {
	mat4 rotation = glm::rotate(mat4(1), glm::radians(azimuth), vec3(0, 1, 0));
	rotation = glm::rotate(rotation, glm::radians(altitude), vec3(1, 0, 0));
	vec3 eye = rotation * vec4(0, 0, -1, 1);

	mat4 view = glm::lookAt(pos + eye, pos, vec3(0, 1, 0));
	return view;
}

mat4 OrthoCamera::getProjectionMatrix() {
	mat4 projection = glm::ortho(
		-0.5f * screenWidth,
		0.5f * screenWidth,
		-0.5f * screenHeight,
		0.5f * screenHeight,
		nearClip, farClip
	);
	projection = glm::scale(projection, 
		vec3(screenHeight * zoom, screenHeight * zoom, 1));
	return projection;
}

void OrthoCamera::setSize(float w, float h) {
	screenWidth = w;
	screenHeight = h;
}

void OrthoCamera::setAzimuth(float az) { 
	azimuth = az;
	while (azimuth < 0.0f) azimuth += 360.0f;
	while (azimuth >= 360.0f) azimuth -= 360.0f;
}

void OrthoCamera::setAltitude(float at) {
	altitude = std::fmin(std::fmax(at, -89.9f), 89.9f);
}

void OrthoCamera::handleMouse() {
	bool orbitButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	bool zoomButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

	if (orbitButton == GLFW_PRESS) {
		if (!mouseOrbit && !mouseZoom) {
			mouseOrbit = true;
			glfwGetCursorPos(window, &clickPos.x, &clickPos.y);
			lastOrientation = { azimuth, altitude };
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	else if (orbitButton == GLFW_RELEASE) {
		if (mouseOrbit) mouseOrbit = false;
	}

	if (zoomButton == GLFW_PRESS) {
		if (!mouseZoom && !mouseOrbit) {
			mouseZoom = true;
			glfwGetCursorPos(window, &clickPos.x, &clickPos.y);
			lastZoom = zoom;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	else if (zoomButton == GLFW_RELEASE) {
		if (mouseZoom) mouseZoom = false;
	}

	if (orbitButton == GLFW_RELEASE && zoomButton == GLFW_RELEASE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (mouseOrbit) {
		glm::dvec2 mousePos;
		glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
		vec2 offset = static_cast<vec2>(mousePos - clickPos);

		setAzimuth(lastOrientation.x - offset.x);
		setAltitude(lastOrientation.y + offset.y);
	} 
	else if (mouseZoom) {
		glm::dvec2 mousePos;
		glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
		vec2 offset = static_cast<vec2>(mousePos - clickPos);

		zoom = std::fmax((lastZoom - offset.y * zoomSensitivity), 1.0f / screenHeight);
	}
}