#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <vector>
#include <glm\glm.hpp>

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using ivec3 = glm::ivec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

class Automata3D {

public:
	struct Vertex {
		Vertex(float x, float y, float z, 
			float nX, float nY, float nZ) : 
			pos(x, y, z), normal(nX, nY, nZ) {}
		glm::vec3 pos;
		glm::vec3 normal;
	};

public:
	Automata3D(ivec3 size, int eL, int eU, int fL, int fU);
	void initRenderData();
	void draw();
	void step();

	void resize(ivec3 newSize);
	void createBox(ivec3 clusterSize);
	void createCross(int thickness, bool omitX = false, 
		bool omitY = false, bool omitZ = false);
	void createCorners(int thickness);
	void createNoise(ivec3 clusterSize);
	int getGeneration();
	ivec3 getSize();

	int eL, eU, fL, fU;
	std::vector<bool> cells;
	std::vector<vec3> blocks;

private:
	void rebuildInstanceArray();
	int countNeighbors(int x, int y, int z);

	GLuint vao, vbo, ebo, ibo;
	ivec3 size;
	int generation;
};