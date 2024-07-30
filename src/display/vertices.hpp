#pragma once

#include "../entry.hpp"
#include "texture_manager.hpp"

using namespace glm;

class VertexArray {
public:
	VertexArray( );
	~VertexArray( );

	void init( );

	void attach( ) const;
	void detach( ) const;

public:
	ALWAYS_INLINE const auto id( ) const { return m_array_id; }

	bool	m_called_in_frame{};
private:
	GLuint	m_array_id{};
};

class RenderBuffer {
public:
	using ValueType = std::vector< float >;

	RenderBuffer( );
	~RenderBuffer( );
	void init( );
	void attach( ValueType buffer, const size_t buffer_size ) const;
	void detach( ) const;
public:
	ALWAYS_INLINE const auto id( ) const { return m_buffer_id; }

	bool	m_called_in_frame{};
private:
	GLuint  m_buffer_id{};
};

// @todo: create color buffer for VertexBaseEntity
class ColorBuffer {

};

class VertexBaseEntity {
public:
	VertexBaseEntity( );
	~VertexBaseEntity( );

	void createObject( 
				RenderBuffer::ValueType vertices_data = { - 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, },
				RenderBuffer::ValueType tex_data = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,},
				const size_t vertices_count = 4u, 
				const size_t tex_count = 4u
	);

	void draw(vec3 render_origin);

	void setTexture( const Texture *texture ) const;

	void setScale( const glm::vec2 size ) const;

	// ALWAYS_INLINE void setOriginZ( const GLfloat originZ ) { m_render_origin.z = originZ / 2.0f; }
public:
	RenderBuffer  m_pos_buffer{}, m_tex_buffer{};
	VertexArray   m_array{};
	size_t		  m_vertices_count{};
	//glm::vec3	  m_render_origin{};
	glm::mat4     m_mvp_matrix{};
};