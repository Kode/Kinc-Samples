#version 450

in vec2 texcoord;

out vec4 FragColor;

uniform sampler2DArray texsampler;

void main() {
	vec4 color = texture(texsampler, vec3(texcoord, texcoord.x * 2));
	FragColor = vec4(color.r, color.g, color.b, 1.0);
}
