#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

out vec2 v_textureCoords;

uniform mat4 u_MvpMatrix;
uniform vec2 u_Scale;
uniform float u_originZ;

void main() {
	vec3 transform = vec3( 
		position.x * u_Scale.x,
		position.y * u_Scale.y,
		u_originZ
	);
	
	gl_Position = u_MvpMatrix * vec4(transform, 1.0);
	v_textureCoords = texCoords;
}