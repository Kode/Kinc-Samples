#version 450

out vec4[4] fragColor;

void main() {
	fragColor[0] = vec4(1.0, 0.0, 0.0, 1.0);
	fragColor[1] = vec4(0.0, 1.0, 0.0, 1.0);
	fragColor[2] = vec4(0.0, 0.0, 1.0, 1.0);
	fragColor[3] = vec4(1.0, 1.0, 0.0, 1.0);
}
