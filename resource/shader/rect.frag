// rect.frag

#version 330 core

in vec2 TexCoords;
out vec4 Color;

uniform vec4 InColor;
uniform vec4 Clip;

void main() {
	if (gl_FragCoord.x >= Clip.x &&
		gl_FragCoord.x <= Clip.z &&
		gl_FragCoord.y >= Clip.y &&
		gl_FragCoord.y <= Clip.w) {

		Color = InColor;

		if (Color.r == 1 && Color.g == 0 && Color.b == 1) {
			discard;
		}
	}
}
