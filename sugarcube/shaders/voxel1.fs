#version 330 core
out vec4 fragColor;

in vec3 vNormal;
in vec3 clipSpacePos;
in float vDistance;

uniform vec4 nearColor;
uniform vec4 farColor;
uniform vec4 innerColor;
uniform vec4 outerColor;
uniform float cameraRampScale;
uniform float cameraRampOffset;
uniform float originRampScale;
uniform float originRampOffset;
uniform float rampMode;

void main() {
	float mixO = vDistance * (1 / originRampScale) + originRampOffset;
	float mixC = clipSpacePos.z * (1 / cameraRampScale) + cameraRampOffset;

	vec4 originRamp = mix(innerColor, outerColor, mixO);
	vec4 cameraRamp = mix(nearColor, farColor, mixC);

	fragColor = mix(originRamp, cameraRamp, rampMode);
}