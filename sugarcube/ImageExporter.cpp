#include "ImageExporter.h"
#include <vector>

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

ImageExporter::ImageExporter(GLsizei width, GLsizei height) :
	w(width), h(height)
{}

void ImageExporter::initialize() {
	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &colorBuffer);
	glGenTextures(1, &depthBuffer);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	oldWidth = viewport[2];
	oldHeight = viewport[3];

	buildFramebuffer();
}

void ImageExporter::buildFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, 
		GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, colorBuffer, 0);

	glBindTexture(GL_TEXTURE_2D, depthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0,
		GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, depthBuffer, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ImageExporter::beginCapture(GLsizei width, GLsizei height) {
	// resize framebuffer to desired output size
	if (width != w || height != h) resize(width, height);

	// save previous glviewport size to restore afterwards
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	oldWidth = viewport[2];
	oldHeight = viewport[3];

	// prepare framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, w, h);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void ImageExporter::saveImage(const char* path, ImageFormats format) {
	std::vector<unsigned char> buf(w * h * 3);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, &buf[0]);

	switch (format) {
	case ImageFormats::PNG:
		stbi_write_png(path, w, h, 3, &buf[0], 3 * w);
		break;
	case ImageFormats::BMP:
		stbi_write_bmp(path, w, h, 3, &buf[0]);
		break;
	case ImageFormats::TGA:
		stbi_write_tga(path, w, h, 3, &buf[0]);
		break;
	case ImageFormats::JPEG:
		stbi_write_jpg(path, w, h, 3, &buf[0], 100);
		break;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, oldWidth, oldHeight);
}

void ImageExporter::resize(GLsizei width, GLsizei height) {
	w = width;
	h = height;
	buildFramebuffer();
}