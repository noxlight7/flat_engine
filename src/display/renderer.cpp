#include "renderer.hpp"

#include "camera.hpp"
#include "texture_manager.hpp"
#include "shaders/shader.hpp"
#include <glm/ext/matrix_clip_space.hpp>


void Renderer::init( GLFWwindow *window ) {
	m_window = window;

	if ( glewInit( ) != GLEW_OK )
		return;

	createTextures( );
	setupVAO( );

}

void Renderer::createTextures( ) {
	loadTexture( HASH( "test" ), "assets/test.png" );
	loadTexture( HASH( "test2" ), "assets/test2.png" );
	loadTexture( HASH( "circle_move" ), "assets/circle_move.png" );
	loadTexture( HASH( "circle_static" ), "assets/circle_static.png" );
}

void Renderer::setupVAO( ) {
	constexpr auto k_scale = 1.0f;
	static GLfloat g_test_vertices[ ] = {
		-k_scale, -k_scale, 1.0, // 1st triangle
		k_scale, -k_scale, 1.0,
		k_scale,  k_scale, 1.0,
		-k_scale,  k_scale, 1.0,
	};

	Shader vertex, fragment;

	vertex.init( "assets/shaders/vertex.sh", GL_VERTEX_SHADER );
	fragment.init( "assets/shaders/frag.sh", GL_FRAGMENT_SHADER );

	g_shader_program->addShaderEntry( vertex );
	g_shader_program->addShaderEntry( fragment );

	g_shader_program->compile( );

	// we should create entity once.
	g_test_entity.createObject( g_test_vertices, 4u );
	g_test_entity2.createObject( g_test_vertices, 4u );
}

void Renderer::drawRectTex( glm::vec2 mins, glm::vec2 maxs, const fnv_edited::value_type tex_entry ) {
	//g_vertex_buffer->addRect(mins, maxs, { 1.0,1.0,1.0,1.0 }, tex_entry);
}

void Renderer::loadTexture( const fnv_edited::value_type entry, const char *path ) {
	Texture texture = Texture( path );

	g_textures->push( entry, texture );
}

void Renderer::destroy( ) {

}

void Renderer::drawFrame( ) {
	// attach shader program.
	g_shader_program->attach( );

	// setup current view matrix.
	g_camera->think( );

	g_shader_program->setUniform4f( "u_Color", { 0.f, 1.f, 0.f, 1.f } );

	const auto texture = g_textures->get( HASH( "test" ) );
	glBindTexture( GL_TEXTURE, texture->getTextureID( ) );
	glActiveTexture( GL_TEXTURE0 );

	g_shader_program->setUniform1i( "u_Texture", 0 );

	g_test_entity.draw( );

	g_shader_program->setUniform4f( "u_Color", { 1.f, 1.f, 1.f, 1.f } );
	g_test_entity2.draw( );
	//printf( "id: test_entity, origin: (%.3f %.3f %.3f)\n", 
	//	g_test_entity.m_render_origin.x, g_test_entity.m_render_origin.y, g_test_entity.m_render_origin.z );

	// detach shader program.
	g_shader_program->detach( );
}

void Renderer::resizeFrameBuffer( GLFWwindow *window, int width, int height ) {
	g_camera->setProjection( width, height );

	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	//printf( "%d %d", width, height );
	//m_proj_matrix = glm::perspective( 45.f, ( float )width / height, .1f, 1000.f );
	//m_proj_matrix = glm::ortho( -width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, 1.f, 10.f );
}