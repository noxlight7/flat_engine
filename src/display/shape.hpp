#pragma once

#include "../entry.hpp"

#include "vertices.hpp"

using Indices = std::vector< uint32_t >;

class IShape {
public:
	virtual void init( Vertices2D vertices, Indices indices ) abstract;
	virtual void destroy( ) abstract;

	virtual void draw( ) abstract;
protected:
	Vertices2D	m_vertices;
	Indices		m_indices;
};

class Shape : public IShape {
public:
	void setup( );

	void init( Vertices2D vertices, Indices indices );
	void destroy( );

	void draw( );
private:
	glm::mat3	m_world_matrix;
	uint32_t	m_uniform_matrix;

	GLuint		m_vertex_buffer;
	GLuint		m_index_buffer;
};