#include "renderer.hpp"

#include "vertex_buffer.hpp"

void Renderer::init( GLFWwindow *window ) {
	m_window = window;

	if ( glewInit( ) != GLEW_OK )
		return;

	createTextures( );
}

void Renderer::createTextures( ) {
	loadTexture( HASH( "test" ), "assets/test.png" );
	loadTexture( HASH( "test2" ), "assets/test2.png" );
}

void Renderer::loadTexture( const fnv_edited::value_type entry, const char *path ) {
	Texture texture = Texture( path );

	g_textures->push( entry, texture );
}

void Renderer::destroy( ) {
	
}

void Renderer::drawFrame( ) {
	glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );

	g_vertex_buffer->draw( );
	g_vertex_buffer->clear( );

	glfwSwapBuffers( m_window );
}

void Renderer::resizeFrameBuffer( GLFWwindow *window, int width, int height ) {
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );

	glLoadIdentity( );
	glOrtho( -width / 2.0, width / 2.0, height / 2.0, -height / 2.0, 0.0, 1.0 );
	glMatrixMode( GL_MODELVIEW );

	glLoadIdentity( );
}