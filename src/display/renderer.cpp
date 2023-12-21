#include "renderer.hpp"

void Renderer::init( GLFWwindow *window ) {
	m_window = window;

	if ( glewInit( ) != GLEW_OK )
		return;
	
}

void Renderer::destroy( ) {

}

void Renderer::drawFrame( ) {
	glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	glfwSwapBuffers( m_window );
}

void Renderer::resizeFrameBuffer( GLFWwindow *window, int width, int height ) {
	
}