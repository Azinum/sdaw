// rect.frag

#version 150

in vec2 TexCoords;
out vec4 Color;

uniform vec4 InColor;
uniform vec4 Clip;

void main() {
	Color = InColor;

	if (Color.r == 1 && Color.g == 0 && Color.b == 1) {
		discard;
	}
}
