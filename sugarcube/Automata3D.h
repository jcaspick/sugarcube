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
	void randomize(ivec3 clusterSize, unsigned int seed = 0);
	void solid(ivec3 clusterSize);
	void draw();
	void step();

	int eL, eU, fL, fU;
	std::vector<bool> cells;
	std::vector<vec3> blocks;
	vec3 size;

private:
	void rebuildInstanceArray();
	int countNeighbors(int x, int y, int z);

	GLuint vao, vbo, ebo, ibo;
};