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

RenderBuffer::RenderBuffer( ) {

}

RenderBuffer::~RenderBuffer( ) {
	glDeleteBuffers( 1, &m_buffer_id );
}

void RenderBuffer::init( ) {
	glGenBuffers( 1, &m_buffer_id );
}

void RenderBuffer::attach( ValueType buffer, const size_t buffer_size ) const {
	glBindBuffer( GL_ARRAY_BUFFER, m_buffer_id );
	glBufferData( GL_ARRAY_BUFFER, 2u * sizeof( float ) * buffer_size, buffer.data( ), GL_DYNAMIC_DRAW );
}

void RenderBuffer::detach( ) const {

}

VertexBaseEntity::VertexBaseEntity( )
	{}

VertexBaseEntity::~VertexBaseEntity( )
	{}

void VertexBaseEntity::createObject( 
	RenderBuffer::ValueType vertices_data, 
	RenderBuffer::ValueType tex_data, 
	const size_t vertices_count, const size_t tex_count 
) {
	m_array.init( );
	m_pos_buffer.init( );
	m_tex_buffer.init( );

	m_pos_buffer.attach( vertices_data, vertices_count );
	m_tex_buffer.attach( tex_data, tex_count );

	m_vertices_count = vertices_count;
}

void VertexBaseEntity::draw( ) {
	// setup mvp matrix.
	m_mvp_matrix = g_camera->worldToScreen( m_render_origin );

	// force mvp matrix.
	g_shader_program->setUniformMatrix4fv( "u_MvpMatrix", m_mvp_matrix );

	// .
	const auto originZ = m_render_origin.z;
	g_shader_program->setUniform1f( "u_originZ", originZ );

	// 
	glEnableVertexAttribArray( 0 );

	glBindBuffer( GL_ARRAY_BUFFER, m_pos_buffer.id( ) );

	glVertexAttribPointer(
		0,
		2,
		GL_FLOAT,
		GL_FALSE,
		8,
		( void * )0
	);

	glEnableVertexAttribArray( 1 );

	glBindBuffer( GL_ARRAY_BUFFER, m_tex_buffer.id( ) );

	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		8,
		( void * )0
	);

	glDrawArrays( GL_QUADS, 0, m_vertices_count );

	glDisableVertexAttribArray( 1 );

	glDisableVertexAttribArray( 0 );
}

void VertexBaseEntity::setTexture( const Texture *texture ) const {
	g_shader_program->setUniform1i( "u_Texture", GET_TEXTURE_SLOT( texture ) );
	glBindTexture( GL_TEXTURE_2D, texture->getTextureID( ) );
}

void VertexBaseEntity::setScale( const glm::vec2 size ) const {
	g_shader_program->setUniform2f( "u_Scale", size );
}