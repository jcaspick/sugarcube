#include "PerspCamera.h"

PerspCamera::PerspCamera(GLFWwindow* window, float screenWidth, float screenHeight) :
	window(window),
	screenWidth(screenWidth),
	screenHeight(screenHeight),
	pos(vec3(0)),
	azimuth(45),
	altitude(35.264f),
	nearClip(5.0f),
	farClip(100.0f),
	mouseOrbit(false),
	mouseZoom(false),
	zoom(20),
	zoomSensitivity(0.05f)
{}

mat4 PerspCamera::getViewMatrix() {
	mat4 rotation = glm::rotate(mat4(1), glm::radians(azimuth), vec3(0, 1, 0));
	rotation = glm::rotate(rotation, glm::radians(altitude), vec3(1, 0, 0));
	vec3 eye = rotation * vec4(0, 0, -zoom, 1);

	mat4 view = glm::lookAt(pos + eye, pos, vec3(0, 1, 0));
	return view;
}

mat4 PerspCamera::getProjectionMatrix(bool flipY) {
	mat4 projection = glm::perspective(45.0f, screenWidth / screenHeight, 
		nearClip, farClip);
	if (flipY) projection = glm::scale(projection, vec3(1, -1, 1));
	return projection;
}

void PerspCamera::setSize(float w, float h) {
	screenWidth = w;
	screenHeight = h;
}

void PerspCamera::setAzimuth(float az) {
	azimuth = az;
	while (azimuth < 0.0f) azimuth += 360.0f;
	while (azimuth >= 360.0f) azimuth -= 360.0f;
}

void PerspCamera::setAltitude(float at) {
	altitude = std::fmin(std::fmax(at, -89.9f), 89.9f);
}

void PerspCamera::handleMouse() {
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

		zoom = lastZoom + offset.y * zoomSensitivity;
	}
}