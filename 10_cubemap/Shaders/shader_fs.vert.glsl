#version 450

in vec2 pos;

out vec2 tex_coord;

void main() {
	tex_coord = pos.xy;

	gl_Position = vec4(pos.xy, 0.0, 1.0);
}
