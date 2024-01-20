#pragma once

#include "../entry.hpp"

#include "shaders/shader.hpp"
#include "texture_manager.hpp"
#include "vertices.hpp"

class Texture;
class Sprite {
public:
	Sprite( const fnv_edited::value_type texture_id );
	~Sprite( );

	void create( );
	void attach( GLuint slot = 0 );
	void detach( );
private:
	Texture *m_texture;
	GLuint	m_texture_id;
};

class IRenderer {
public:
	virtual void init( GLFWwindow *window ) abstract;
	virtual void destroy( ) abstract;

	virtual void drawFrame( ) abstract;
	virtual void resizeFrameBuffer( GLFWwindow *window, int width, int height ) abstract;
protected:
	GLFWwindow *m_window;
};

class Renderer : public IRenderer {
public:
	void init( GLFWwindow *window );
	void destroy( );

	void setupVAO( );

	void drawRectTex(glm::vec2 mins, glm::vec2 maxs, const fnv_edited::value_type tex_entry);
	
	void drawFrame( );
	void resizeFrameBuffer( GLFWwindow *window, int width, int height );

	void createTextures( );

	VertexBaseEntity g_test_entity{};
	VertexBaseEntity g_test_entity2{};
protected:
	void loadTexture( const fnv_edited::value_type entry, const char *path );
protected:
};

inline const auto g_renderer 
	= std::make_shared< Renderer >( );