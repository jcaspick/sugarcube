#include "Automata3D.h"

#include <iostream>
#include <ctime>

Automata3D::Automata3D(ivec3 size, int eL, int eU, int fL, int fU) :
	cells(size.x * size.y * size.z, 0),
	eL(eL), eU(eU), fL(fL), fU(fU),
	size(size),
	generation(1)
{
	srand(time(NULL));
}

void Automata3D::draw() {
	glBindVertexArray(vao);
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, blocks.size());
}

void Automata3D::step() {
	std::vector<bool> nextFrame(size.x * size.y * size.z, false);

	for (int x = 0; x < size.x; x++) {
		for (int y = 0; y < size.y; y++) {
			for (int z = 0; z < size.z; z++) {
				int neighbors = countNeighbors(x, y, z);
				// live cell
				if (cells[z * size.x * size.y + y * size.x + x]) {
					if (neighbors >= eL && neighbors <= eU)
						nextFrame[z * size.x * size.y + y * size.x + x] = true;
				}
				// dead cell
				else {
					if (neighbors >= fL && neighbors <= fU)
						nextFrame[z * size.x * size.y + y * size.x + x] = true;
				}
			}
		}
	}

	cells = nextFrame;
	generation++;
	rebuildInstanceArray();
}

int Automata3D::countNeighbors(int x, int y, int z) {
	int count = 0;
	for (int ix = x - 1; ix <= x + 1; ix++) {
		for (int iy = y - 1; iy <= y + 1; iy++) {
			for (int iz = z - 1; iz <= z + 1; iz++) {
				if (ix == x && iy == y && iz == z) continue;
				if (ix < 0 || ix >= size.x ||
					iy < 0 || iy >= size.y ||
					iz < 0 || iz >= size.z) continue;
				if (cells[iz * size.x * size.y + iy * size.x + ix]) {
					count++;
				}
			}
		}
	}
	return count;
}

void Automata3D::resize(ivec3 newSize) {
	size = newSize;
	cells.resize(newSize.x * newSize.y * newSize.z, false);
}

void Automata3D::createBox(ivec3 clusterSize) {
	if (clusterSize.x > size.x ||
		clusterSize.y > size.y ||
		clusterSize.z > size.z) return;

	std::fill(cells.begin(), cells.end(), false);
	ivec3 offset(
		(size.x - clusterSize.x) * 0.5f,
		(size.y - clusterSize.y) * 0.5f,
		(size.z - clusterSize.z) * 0.5f);

	for (int x = offset.x; x < size.x - offset.x; x++) {
		for (int y = offset.y; y < size.y - offset.y; y++) {
			for (int z = offset.z; z < size.z - offset.z; z++) {
				cells[z * size.x * size.y + y * size.x + x] = true;
			}
		}
	}

	generation = 1;
	rebuildInstanceArray();
}

void Automata3D::createCross(int thickness, bool omitX, bool omitY, bool omitZ) {
	std::fill(cells.begin(), cells.end(), false);

	for (int x = 0; x < size.x; x++) {
		for (int y = 0; y < size.y; y++) {
			for (int z = 0; z < size.z; z++) {
				int idx = z * size.x * size.y + y * size.x + x;
				float t = (float)thickness * 0.5f;
				bool cX = x + t >= ((float)size.x - 1) * 0.5f && x - t <= ((float)size.x - 1) * 0.5f;
				bool cY = y + t >= ((float)size.y - 1) * 0.5f && y - t <= ((float)size.y - 1) * 0.5f;
				bool cZ = z + t >= ((float)size.z - 1) * 0.5f && z - t <= ((float)size.z - 1) * 0.5f;
				if (!omitZ && cX && cY) cells[idx] = true;
				if (!omitY && cX && cZ) cells[idx] = true;
				if (!omitX && cY && cZ) cells[idx] = true;
			}
		}
	}

	generation = 1;
	rebuildInstanceArray();
}

void Automata3D::createCorners(int thickness) {
	std::fill(cells.begin(), cells.end(), false);

	for (int x = 0; x < size.x; x++) {
		for (int y = 0; y < size.y; y++) {
			for (int z = 0; z < size.z; z++) {
				int idx = z * size.x * size.y + y * size.x + x;
				bool cX = x < thickness || x >= size.x - thickness;
				bool cY = y < thickness || y >= size.y - thickness;
				bool cZ = z < thickness || z >= size.z - thickness;
				if (cX && cY && cZ) cells[idx] = true;
			}
		}
	}

	generation = 1;
	rebuildInstanceArray();
}

void Automata3D::createNoise(ivec3 clusterSize) {
	if (clusterSize.x > size.x ||
		clusterSize.y > size.y ||
		clusterSize.z > size.z) return;

	std::fill(cells.begin(), cells.end(), false);

	ivec3 offset(
		(size.x - clusterSize.x) * 0.5f,
		(size.y - clusterSize.y) * 0.5f,
		(size.z - clusterSize.z) * 0.5f);

	for (int x = offset.x; x < size.x - offset.x; x++) {
		for (int y = offset.y; y < size.y - offset.y; y++) {
			for (int z = offset.z; z < size.y - offset.z; z++) {
				cells[z * size.x * size.y + y * size.x + x] = rand() % 2;
			}
		}
	}

	generation = 1;
	rebuildInstanceArray();
}

void Automata3D::rebuildInstanceArray() {
	blocks.clear();

	for (int x = 0; x < size.x; x++) {
		for (int y = 0; y < size.y; y++) {
			for (int z = 0; z < size.z; z++) {
				if (!cells[z * size.x * size.y + y * size.x + x])
					continue;
				blocks.push_back(vec3(
					x - (0.5f * static_cast<float>(size.x - 1)), 
					y - (0.5f * static_cast<float>(size.y - 1)), 
					z - (0.5f * static_cast<float>(size.z - 1))));
			}
		}
	}

	if (blocks.size() == 0) return;

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, ibo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * blocks.size(), &blocks[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glVertexAttribDivisor(2, 1);
}

void Automata3D::initRenderData() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	Vertex cubeVertices[24] = {
		// -Y
		Vertex(-0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f),
		Vertex( 0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f),
		Vertex(-0.5f, -0.5f,  0.5f,     0.0f, -1.0f, 0.0f),
		Vertex( 0.5f, -0.5f,  0.5f,     0.0f, -1.0f, 0.0f),
		// -Z
		Vertex( 0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f),
		Vertex(-0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f),
		Vertex( 0.5f,  0.5f, -0.5f,     0.0f, 0.0f, -1.0f),
		Vertex(-0.5f,  0.5f, -0.5f,     0.0f, 0.0f, -1.0f),
		// -X
		Vertex(-0.5f, -0.5f, -0.5f,     -1.0f, 0.0f, 0.0f),
		Vertex(-0.5f, -0.5f,  0.5f,     -1.0f, 0.0f, 0.0f),
		Vertex(-0.5f,  0.5f, -0.5f,     -1.0f, 0.0f, 0.0f),
		Vertex(-0.5f,  0.5f,  0.5f,     -1.0f, 0.0f, 0.0f),
		// +Z
		Vertex(-0.5f, -0.5f,  0.5f,     0.0f, 0.0f, 1.0f),
		Vertex( 0.5f, -0.5f,  0.5f,     0.0f, 0.0f, 1.0f),
		Vertex(-0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f),
		Vertex( 0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f),
		// +X
		Vertex( 0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f),
		Vertex( 0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f),
		Vertex( 0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f),
		Vertex( 0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f),
		// +Y
		Vertex(-0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f),
		Vertex( 0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f),
		Vertex(-0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f),
		Vertex( 0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f)
	};

	GLuint cubeIndices[36] = {
		0, 1, 2,
		2, 1, 3,
		4, 5, 6,
		6, 5, 7,
		8, 9, 10,
		10, 9, 11,
		12, 13, 14,
		14, 13, 15,
		16, 17, 18,
		18, 17, 19,
		20, 21, 22,
		22, 21, 23
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(Vertex), &cubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
		(void*)offsetof(Vertex, normal));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &cubeIndices, GL_STATIC_DRAW);
}

int Automata3D::getGeneration() { return generation; }
ivec3 Automata3D::getSize() { return size; }