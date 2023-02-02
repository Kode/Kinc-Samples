#version 450

uniform sampler2D tex;

in vec2 tex_coord;
out vec4 fragColor;

void main() {
	fragColor = textureLod(tex, tex_coord, 4.0);
}
