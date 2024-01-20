#include "vertices.hpp"
#include "camera.hpp"
#include "shaders/shader.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

VertexArray::VertexArray( ) {

}

VertexArray::~VertexArray( ) {
	glDeleteVertexArrays( 1, &m_array_id );
}

void VertexArray::init( ) {
	glGenVertexArrays( 1, &m_array_id );
}

void VertexArray::attach( ) const {
	glBindVertexArray( m_array_id );
}

void VertexArray::detach( ) const {
	glBindVertexArray( 0 );
}

VertexBuffer::VertexBuffer( ) {

}

VertexBuffer::~VertexBuffer( ) {
	glDeleteBuffers( 1, &m_buffer_id );
}

void VertexBuffer::init( ) {
	glGenBuffers( 1, &m_buffer_id );
}

void VertexBuffer::attach( float * vertices_data, const size_t vertices_count ) const {
	glBindBuffer( GL_ARRAY_BUFFER, m_buffer_id );
	glBufferData( GL_ARRAY_BUFFER, 3 * sizeof( GLfloat ) * vertices_count, vertices_data, GL_DYNAMIC_DRAW );
}

void VertexBuffer::detach( ) const {

}

VertexBaseEntity::VertexBaseEntity( )
	{}

VertexBaseEntity::~VertexBaseEntity( )
	{}

void VertexBaseEntity::createObject( float *vertices_data, const size_t vertices_count ) {
	m_array.init( );
	m_buffer.init( );

	m_buffer.attach( vertices_data, vertices_count );

	m_vertices_count = vertices_count;
}

void VertexBaseEntity::draw( ) {
	// setup mvp matrix.
	m_mvp_matrix = g_camera->worldToScreen( m_render_origin );

	// force mvp matrix.
	g_shader_program->setUniformMatrix4fv( "u_MvpMatrix", m_mvp_matrix );

	// 
	glEnableVertexAttribArray( 0 );

	glBindBuffer( GL_ARRAY_BUFFER, m_buffer.id( ) );

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		( void * )0
	);

	glDrawArrays( GL_QUADS, 0, m_vertices_count );
}