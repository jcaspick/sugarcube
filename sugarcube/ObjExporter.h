#pragma once
#include <glm\glm.hpp>

#include <vector>

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using ivec3 = glm::ivec3;
using vec4 = glm::vec4;

class ObjExporter {

public:
	struct Vertex {
		vec3 position;
		vec3 normal;
	};

public:
	ObjExporter();
	ObjExporter(std::vector<bool> data, ivec3 size);

	void load(std::vector<bool> data, ivec3 size);
	void exportObj();

private:
	int coordsToIndex(int x, int y, int z);
	bool isEmpty(int x, int y, int z);
	void addFace(vec3 center, vec3 normal, std::vector<Vertex>& faces);

	std::vector<bool> data;
	ivec3 size;
};