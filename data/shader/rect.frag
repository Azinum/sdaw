// rect.frag

#version 150

in vec2 TexCoord;
out vec4 Color;

uniform vec4 InColor;
uniform vec4 InBorderColor;
uniform float Thickness;
uniform bool DrawBorder;
uniform vec2 RectSize;
uniform vec4 Clip;

void main() {
	// if (gl_FragCoord.x >= Clip.x && gl_FragCoord.x <= Clip.z && gl_FragCoord.y >= Clip.y && gl_FragCoord.y <= Clip.w) {
	// TODO(lucas): FIXME border sizing isn't exactly as specified
		if (
			DrawBorder && (
				(TexCoord.x <= (Thickness / (RectSize.x)) || TexCoord.x >= 1.0 - Thickness / (RectSize.x)) ||
				(TexCoord.y <= (Thickness / RectSize.x) * (RectSize.x / RectSize.y) ||
				(TexCoord.y >= 1.0 - (Thickness / RectSize.x) * (RectSize.x / RectSize.y)))
			)
		) {
			Color = InBorderColor;
		}
		else {
			Color = InColor;
		}
		if ((Color.r == 1 && Color.g == 0 && Color.b == 1) || Color.a < 0.1) {
			discard;
		}
	// }
	// else {
	// 	discard;
	// }
}
