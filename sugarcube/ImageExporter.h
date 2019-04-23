#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum class ImageFormats {
	BMP,
	PNG,
	TGA,
	JPEG
};

class ImageExporter {
public:
	ImageExporter(GLsizei width, GLsizei height);
	void initialize();
	void buildFramebuffer();
	void beginCapture(GLsizei width, GLsizei height);
	void saveImage(const char* path, ImageFormats format);
	void resize(GLsizei width, GLsizei height);

private:
	GLsizei w, h;
	GLsizei oldWidth, oldHeight;
	GLuint fbo;
	GLuint colorBuffer, depthBuffer;
};