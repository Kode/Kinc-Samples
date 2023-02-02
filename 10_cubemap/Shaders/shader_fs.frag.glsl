#version 450

uniform samplerCube tex;

in vec2 tex_coord;
out vec4 fragColor;

void main() {
	fragColor = textureLod(tex, vec3(tex_coord, 0.0), 0.0);
}
