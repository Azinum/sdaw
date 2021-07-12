// text.frag

#version 150

in vec2 TexCoord;
out vec4 Color;

uniform sampler2D Texture;
uniform vec2 Offset;
uniform vec2 Range;
uniform vec4 Tint;
uniform vec4 Clip;

void main() {
	Color = texture(Texture, (TexCoord * Range) + Offset);
	if (Color.r == 1 && Color.g == 0 && Color.b == 1) {
		discard;
	}
	else {
		Color *= Tint;
	}
}
