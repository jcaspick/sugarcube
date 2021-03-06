#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 offset;

out vec3 vNormal;
out float vDistance;
out vec3 clipSpacePos;

uniform mat4 view;
uniform mat4 projection;
uniform int smoothLight;

void main() {
	vec4 vPos = vec4(pos + offset, 1);
	vNormal = normal;
	vDistance = length(pos * smoothLight + offset);
	clipSpacePos = (projection * view * vec4(pos * smoothLight + offset, 1)).xyz;

	gl_Position = projection * view * vPos;
}