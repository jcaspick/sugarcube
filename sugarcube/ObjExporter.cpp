#include "ObjExporter.h"
#include <string>
#include <iostream>
#include <fstream>

ObjExporter::ObjExporter() {}

ObjExporter::ObjExporter(std::vector<bool> data, ivec3 size) :
	data(data),
	size(size)
{}

void ObjExporter::load(std::vector<bool> data, ivec3 size) {
	this->data = data;
	this->size = size;
}

void ObjExporter::exportObj() {
	if (data.size() == 0) {
		std::cout << "Can't export, no data" << std::endl;
		return;
	}

	// create a list of vertices of visible faces
	// skip those that are obscured by neighboring cubes
	std::vector<Vertex> vertices;
	for (int x = 0; x < size.x; x++) {
		for (int y = 0; y < size.y; y++) {
			for (int z = 0; z < size.z; z++) {
				// skip empty cells
				if (isEmpty(x, y, z)) continue;

				// get coordinates of center point of cube
				vec3 center(
					static_cast<float>(x) - static_cast<float>(size.x - 1) * 0.5f,
					static_cast<float>(y) - static_cast<float>(size.y - 1) * 0.5f,
					static_cast<float>(z) - static_cast<float>(size.z - 1) * 0.5f
				);

				// check each orthographic neighbor
				if (isEmpty(x - 1, y, z)) addFace(center, vec3(-1,  0,  0), vertices);
				if (isEmpty(x + 1, y, z)) addFace(center, vec3( 1,  0,  0), vertices);
				if (isEmpty(x, y - 1, z)) addFace(center, vec3( 0, -1,  0), vertices);
				if (isEmpty(x, y + 1, z)) addFace(center, vec3( 0,  1,  0), vertices);
				if (isEmpty(x, y, z - 1)) addFace(center, vec3( 0,  0, -1), vertices);
				if (isEmpty(x, y, z + 1)) addFace(center, vec3( 0,  0,  1), vertices);
			}
		}
	}

	// write obj file
	std::ofstream obj;
	obj.open("export.obj");

	obj << "# vertices\n";
	for (int i = 0; i < vertices.size(); i++) {
		obj << "v " << std::to_string(vertices[i].position.x)
			<< "  " << std::to_string(vertices[i].position.y)
			<< "  " << std::to_string(vertices[i].position.z) << "\n";
	}

	obj << "\n# vertex normals\n";
	for (int i = 0; i < vertices.size(); i++) {
		obj << "vn " << std::to_string(vertices[i].normal.x)
			<< "  " << std::to_string(vertices[i].normal.y)
			<< "  " << std::to_string(vertices[i].normal.z) << "\n";
	}

	obj << "\n# faces\n";
	for (int i = 0; i < vertices.size(); i += 4) {
		obj << "f "
			<< std::to_string(i + 1) << "//" << std::to_string(i + 1) << " "
			<< std::to_string(i + 3) << "//" << std::to_string(i + 3) << " "
			<< std::to_string(i + 4) << "//" << std::to_string(i + 4) << " "
			<< std::to_string(i + 2) << "//" << std::to_string(i + 2) << " ";
		obj << "\n";
	}

	obj.close();
}

int ObjExporter::coordsToIndex(int x, int y, int z) {
	return (z * size.x * size.y + y * size.x + x);
}

bool ObjExporter::isEmpty(int x, int y, int z) {
	if (x < 0 || x >= size.x ||
		y < 0 || y >= size.y ||
		z < 0 || z >= size.z)
		return true;

	return (!data[coordsToIndex(x, y, z)]);
}

void ObjExporter::addFace(vec3 center, vec3 normal, std::vector<Vertex>& vertices) {
	// dot product of normal determines face orientation
	// face orientation determines choice of tangents
	// tangents are premultiplied by 0.5f for brevity
	vec3 tangent1, tangent2;
	if (glm::dot(normal, vec3(1, 0, 0)) != 0) {
		tangent1 = vec3(0, 0.5f, 0);
		tangent2 = vec3(0, 0,    0.5f);
	}
	else if (glm::dot(normal, vec3(0, 1, 0)) != 0) {
		tangent2 = vec3(0.5f, 0, 0);
		tangent1 = vec3(0,    0, 0.5f);
	}
	else if (glm::dot(normal, vec3(0, 0, 1)) != 0) {
		tangent1 = vec3(0.5f, 0,    0);
		tangent2 = vec3(0,    0.5f, 0);
	}

	// this weird bit of code is a quick way to reverse the winding
	// order of a face if the normal is in a negative direction
	if (normal.x + normal.y + normal.z < 0) {
		tangent1 *= -1.0f;
	}

	// use tangents and normal vector to construct 4 vertices of face
	Vertex v1, v2, v3, v4;
	v1.position = center - tangent1 - tangent2 + normal * 0.5f;
	v2.position = center - tangent1 + tangent2 + normal * 0.5f;
	v3.position = center + tangent1 - tangent2 + normal * 0.5f;
	v4.position = center + tangent1 + tangent2 + normal * 0.5f;

	v1.normal = normal;
	v2.normal = normal;
	v3.normal = normal;
	v4.normal = normal;

	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v4);
}