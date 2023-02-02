#version 450

in vec3 pos;
in vec2 tex;

out vec2 tex_coord;

uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(pos, 1.0);

	tex_coord = tex;
}
