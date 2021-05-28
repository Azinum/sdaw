// rect.vert

#version 130

in vec4 Vertex;

out vec2 TexCoords;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main() {
	TexCoords = Vertex.zw;
	gl_Position = Projection * View * Model * vec4(Vertex.xy, 0, 1.0);
}
