#include "shader.hpp"

Shader::~Shader( ) {
	if ( m_shader )
		glDeleteShader( m_shader );
}

bool Shader::compile( const char *shader, GLenum type ) {
	m_shader = glCreateShader( type );

	const char *code = shader;

	glShaderSource( m_shader, 1, &code, 0 );
	glCompileShader( m_shader );
	glGetShaderiv( m_shader, GL_COMPILE_STATUS, &m_compiled );

	if ( !m_compiled )
		printf( "shader not compiled\n" );

	return m_compiled;
}