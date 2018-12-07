#include "PPM_Exporter.h"
#include <iostream>
#include <fstream>
#include <string>

PPM_Exporter::PPM_Exporter(GLsizei width, GLsizei height) :
	w(width), h(height)
{}

void PPM_Exporter::initialize() {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	oldWidth = viewport[2];
	oldHeight = viewport[3];

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0,
		GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, color, 0);

	glGenTextures(1, &depthStencil);
	glBindTexture(GL_TEXTURE_2D, depthStencil);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0,
		GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, depthStencil, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PPM_Exporter::beginCapture() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, w, h);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void PPM_Exporter::saveImage() {
	unsigned char* image = (unsigned char*)malloc(sizeof(unsigned char) * 3 * w * h);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);

	std::ofstream ifile;
	ifile.open("imageExport.ppm");
	ifile << "P6 " << std::to_string(w) << " " << std::to_string(h) << " 255 ";
	ifile.write((char*)&image[0], 3 * w * h);
	ifile.close();

	free(image);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, oldWidth, oldHeight);
}