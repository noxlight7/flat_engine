#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_textureCoords;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main() {
	color = texture(u_Texture, v_textureCoords) * u_Color;
}