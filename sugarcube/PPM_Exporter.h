#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>

class PPM_Exporter {

public:
	PPM_Exporter(GLsizei width, GLsizei height);
	void initialize();
	void beginCapture();
	void saveImage();

private:
	GLsizei w, h;
	GLsizei oldWidth, oldHeight;

	GLuint fbo;
	GLuint color, depthStencil;
};