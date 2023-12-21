#pragma once

#include "../entry.hpp"

class Shader {
public:
	Shader( ) = default;
	~Shader( );

	bool compile( const char *shader, GLenum type );
private:
	GLuint m_shader;
	GLint  m_compiled;
};