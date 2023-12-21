#include "shape.hpp"

void Shape::init( Vertices2D vertices, Indices indices ) {
	m_vertices = vertices;
	m_indices = indices;

	glGenBuffers( 1, &m_vertex_buffer );
	glBindBuffer( GL_ARRAY_BUFFER, m_vertex_buffer );

	glBufferData( GL_ARRAY_BUFFER, vertices.size( ) * sizeof( vert2 ), &vertices[ 0 ], GL_STATIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glGenBuffers( 1, &m_index_buffer );
	glBindBuffer( GL_ARRAY_BUFFER, m_index_buffer );
	glBufferData( GL_ARRAY_BUFFER, indices.size( ) * sizeof( unsigned int ), &indices[ 0 ], GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void Shape::draw( ) {
	glBindBuffer( GL_ARRAY_BUFFER, m_vertex_buffer );

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( vert2 ), NULL );

	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( vert2 ), ( void * )sizeof( glm::vec2 ) );

	glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof( vert2 ), ( void * )sizeof( glm::vec4 ) );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glUniformMatrix3fv( m_uniform_matrix, 1, GL_FALSE, &( m_world_matrix[ 0 ][ 0 ] ) );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_index_buffer );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );

	glDrawElements( GL_TRIANGLES, m_indices.size( ), GL_UNSIGNED_INT, NULL );

	glDisableVertexAttribArray( 0 );
	glDisableVertexAttribArray( 1 );
	glDisableVertexAttribArray( 2 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}