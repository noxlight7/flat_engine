#version 460 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D u_texture;
uniform vec4 u_color = vec4(1.0); // белый цвет по умолчанию

void main() {
    FragColor = texture(u_texture, TexCoord) * u_color;
}