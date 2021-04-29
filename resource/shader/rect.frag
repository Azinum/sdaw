// rect.frag

#version 330 core

in vec2 TexCoords;
out vec4 Color;

uniform vec4 InColor;

void main() {
	Color = InColor;
	if (Color.r == 1 && Color.g == 0 && Color.b == 1) {
		discard;
	}
}
