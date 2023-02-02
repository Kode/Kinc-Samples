#version 450

in vec2 pos;

out vec2 tex_coord;

void main() {
	const vec2 madd = vec2(0.5, 0.5);
	tex_coord = pos.xy * madd + madd;

	gl_Position = vec4(pos.xy, 0.0, 1.0);
}
