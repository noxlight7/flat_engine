#pragma once

#include "../entry.hpp"

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

class VertexBuffer {
public:
	VertexBuffer( );
	~VertexBuffer( );
	void init( );
	void attach( float * vertices_data, const size_t vertices_count ) const;
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

	void createObject( float *vertices_data, const size_t vertices_count );
	void draw( );

	VertexBuffer  m_buffer{};
	VertexArray   m_array{};
	size_t		  m_vertices_count{};
	glm::vec3	  m_render_origin{};
	glm::mat4     m_mvp_matrix{};
};