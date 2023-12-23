#pragma once

#include "../entry.hpp"

class Shader {
public:
	Shader( ) = default;
	~Shader( );

	bool compile( const char *shader, GLenum type );
	void attach( GLuint shader_program );

	ALWAYS_INLINE bool isValid( ) const { return m_compiled > 0; }
private:
	GLuint m_shader;
	GLint  m_compiled;
};