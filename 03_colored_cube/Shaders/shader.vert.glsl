#version 450

in vec3 pos;
in vec3 col;

out vec3 fragment_color;

uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(pos, 1.0);

	fragment_color = col;
}
