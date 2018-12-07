#version 330 core
out vec4 fragColor;

in vec3 vNormal;
in vec3 clipSpacePos;

uniform float rampScale;
uniform float rampOffset;
uniform vec4 nearColor;
uniform vec4 farColor;

void main() {
	float t = (clipSpacePos.z * rampScale + rampOffset);
	fragColor = mix(nearColor, farColor, t);
}