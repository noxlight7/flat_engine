#include "shader.hpp"
#include <glm/gtc/type_ptr.hpp>

Shader::Shader( ) 
	{}
Shader::~Shader( )
	{}

void Shader::init( const std::string_view &path, const GLuint shader_type ) {
	// open shader`s file.
	std::ifstream entry( path.data( ), std::ios::in );
	if ( !entry.is_open( ) )
		PUSH_EXCEPTION( "failed to open %s`s shader\n", path.data( ) );

	// read shader source.
	std::stringstream shader_source_entry{};
	shader_source_entry << entry.rdbuf( );

	// close reader.
	entry.close( );

	// copy shader`s code into string.
	std::string shader_source{ shader_source_entry.str( ) };

	// create shader.
	m_shader_id = glCreateShader( shader_type );

	// add source to shader.
	char const *source_ptr = shader_source.c_str( );
	glShaderSource( m_shader_id, 1, &source_ptr, nullptr );
	glCompileShader( m_shader_id );

	glGetShaderiv( m_shader_id, GL_COMPILE_STATUS, &m_status );
	if ( !m_status ) {
		printf( "shader %s`s compilation failed.\n", path.data( ) );
#ifdef _DEBUG
		int length;
		glGetShaderiv( m_shader_id, GL_INFO_LOG_LENGTH, &length );
		char *message = ( char * )alloca( length * sizeof( char ) );
		glGetShaderInfoLog( m_shader_id, length, &length, message );

		std::cout << message << std::endl;
#endif
	}
}

ShaderProgram::ShaderProgram( )
	{}
ShaderProgram::~ShaderProgram( )
	{}

void ShaderProgram::addShaderEntry( const Shader entry ) {
	m_shader_entries.push_back( entry.id( ) );
}

void ShaderProgram::compile( ) {
	m_program_id = glCreateProgram( );
	for ( auto &entry : m_shader_entries )
		glAttachShader( m_program_id, entry );

	glLinkProgram( m_program_id );
	glValidateProgram( m_program_id );

	for ( auto &entry : m_shader_entries )
		glDeleteShader( entry );
}

void ShaderProgram::setUniform1i( const std::string_view &uniform, const GLint value ) const {
	const auto location = getUniformLocation( uniform );

	glUniform1i( location, value );
}

void ShaderProgram::setUniform1f( const std::string_view &uniform, const GLfloat value ) const {
	const auto location = getUniformLocation( uniform );

	glUniform1f( location, value );
};

void ShaderProgram::setUniform2f( const std::string_view &uniform, const glm::vec2 value ) const {
	const auto location = getUniformLocation( uniform );

	glUniform2f( location, value.x, value.y );
}

void ShaderProgram::setUniform4f( const std::string_view &uniform, const glm::vec4 value ) const {
	const auto location = getUniformLocation( uniform );

	glUniform4f( location, value.x, value.y, value.z, value.w );
}

void ShaderProgram::setUniformMatrix4fv( const std::string_view &uniform, const glm::mat4 value ) const {
	const auto location = getUniformLocation( uniform );

	glUniformMatrix4fv( location, 1, GL_FALSE, glm::value_ptr( value ) );
}

const GLint ShaderProgram::getUniformLocation( const std::string_view &uniform ) const {
	const auto res = glGetUniformLocation( m_program_id, uniform.data( ) );

	return res;
}
