#version 450

in vec2 texcoord;
out vec4 frag;
uniform sampler2D texsampler;

void main() {
	vec4 color = texture(texsampler, texcoord);
	frag = vec4(color.r, color.g, color.b, 1.0);
}
