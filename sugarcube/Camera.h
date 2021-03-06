#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

using mat4 = glm::mat4;

class Camera {

public:
	virtual mat4 getViewMatrix() = 0;
	virtual mat4 getProjectionMatrix(bool flipY = false) = 0;
	virtual void setSize(float w, float h) = 0;
	virtual void setAzimuth(float az) = 0;
	virtual void setAltitude(float at) = 0;
};