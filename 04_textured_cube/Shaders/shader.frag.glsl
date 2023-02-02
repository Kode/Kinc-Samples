#version 450

in vec2 tex_coord;

out vec4 FragColor;

uniform sampler2D texsampler;

void main() {
	FragColor = texture(texsampler, tex_coord);
}
