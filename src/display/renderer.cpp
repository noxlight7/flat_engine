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
	loadTexture( HASH( "test" ), "assets/sprites/test.png" );
	loadTexture( HASH( "test2" ), "assets/sprites/test2.png" );
	loadTexture( HASH( "circle_move" ), "assets/sprites/circle_move.png" );
	loadTexture( HASH( "circle_static" ), "assets/sprites/circle_static.png" );
}

void Renderer::setupVAO( ) {
	Shader vertex, fragment;

	vertex.init( "assets/shaders/vertex.sh", GL_VERTEX_SHADER );
	fragment.init( "assets/shaders/frag.sh", GL_FRAGMENT_SHADER );

	g_shader_program->addShaderEntry( vertex );
	g_shader_program->addShaderEntry( fragment );

	g_shader_program->compile( );

	// we should create entity once.
	// @todo: create base entity class and move this into entities list
	m_rectangle_entity.createObject( );
}

void Renderer::loadTexture( const fnv1a::value_type entry, const char *path ) {
	Texture texture = Texture( path );

	g_textures->push( entry, texture );
}

void Renderer::destroy( ) {

}


void Renderer::prepareFrame() {
	// attach blend channel to renderer.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// attach shader program.
	g_shader_program->attach();

	// setup current view matrix.
	g_camera->think();

	// @todo: make color mix w tex color.
	g_shader_program->setUniform4f("u_Color", { 1.f, 1.f, 1.f, 1.f });
}

void Renderer::drawFrame( ) {
	// detach shader program.
	g_shader_program->detach( );
}

void Renderer::resizeFrameBuffer( GLFWwindow *window, int width, int height ) {
	g_camera->setProjection( width, height );

	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
}

void Renderer::drawRectangleTextured(
	float rect_width,
	float rect_height,
	Texture* texture,
	vec3 origin) {
	m_rectangle_entity.setScale({ rect_width, rect_height });
	m_rectangle_entity.setTexture(texture);

	m_rectangle_entity.draw(origin);
}

void Renderer::drawRectangleTextured(
	RectangleFormData rect,
	Texture* texture,
	vec3 origin) {
	drawRectangleTextured(rect.m_width, rect.m_height, texture, origin);
}