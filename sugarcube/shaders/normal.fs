#version 330 core
out vec4 fragColor;

in vec3 vNormal;
in vec3 clipSpacePos;
in float vDistance;

uniform vec4 xColor;
uniform vec4 yColor;
uniform vec4 zColor;
uniform vec4 ambientColor;
uniform vec4 lightColor;
uniform vec3 lightDir;
uniform float normalMix;
uniform float lightMix;

void main() {
	vec4 normalColors = xColor * abs(vNormal.x) + yColor * abs(vNormal.y) + zColor * abs(vNormal.z);
	vec4 lit = max(dot(normalize(-lightDir), vNormal), 0) * lightColor;

	fragColor = mix(ambientColor, normalColors, normalMix) + lit * lightMix;
}