#pragma once

#include "../entry.hpp"

class Shader {
public:
	Shader( );
	~Shader( );

	void init( const std::string_view &path, const GLuint shader_type );

	ALWAYS_INLINE const auto id( ) const { return m_shader_id; }

	ALWAYS_INLINE const auto type( ) const { return m_shader_type; }
private:
	GLuint m_shader_id{};
	GLuint m_shader_type{};
	GLint  m_status{};
};

class ShaderProgram {
public:
	ShaderProgram( );
	~ShaderProgram( );

	void addShaderEntry( const Shader entry );
	void compile( );

	void setUniform1i( const std::string_view &uniform, const GLint value ) const;

	void setUniform4f( const std::string_view &uniform, const glm::vec4 value ) const;
	
	void setUniformMatrix4fv( const std::string_view &uniform, const glm::mat4 value ) const;
public:
	ALWAYS_INLINE void attach( ) const {
		glUseProgram( m_program_id );
	}

	ALWAYS_INLINE void detach( ) const {
		glUseProgram( 0 );
	}

	ALWAYS_INLINE const auto id( ) const { return m_program_id; }
private:
	const GLint getUniformLocation( const std::string_view &uniform ) const;

	GLuint					m_program_id{};
	std::vector< GLuint >   m_shader_entries{};
};

inline const auto g_shader_program
		= std::make_shared< ShaderProgram >( );