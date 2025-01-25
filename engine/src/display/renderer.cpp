#include "renderer.hpp"

#include "camera.hpp"
#include "texture_manager.hpp"
#include "shaders/shader.hpp"
#include <glm/ext/matrix_clip_space.hpp>

void Renderer::init( GLFWwindow *window, std::vector<std::string> textures_names ) {
	m_window = window;

	if ( glewInit( ) != GLEW_OK )
		return;

	createTextures( );
	for (const auto &texture_name : textures_names ) {
		loadTexture(texture_name);
	}
	setupVAO( );
}

void Renderer::createTextures( ) {

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

void Renderer::loadTexture(std::string path) {
	g_textures->push( std::move(path) );
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
