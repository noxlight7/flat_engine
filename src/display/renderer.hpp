#pragma once

#include "../entry.hpp"

#include "shaders/shader.hpp"
#include "texture_manager.hpp"
#include "vertices.hpp"
#include "physics/base_physics.hpp"

class Texture;
class Sprite {
public:
	Sprite( const fnv1a::value_type texture_id );
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
	virtual void prepareFrame() abstract;
	virtual void resizeFrameBuffer( GLFWwindow *window, int width, int height ) abstract;

	virtual void drawRectangleTextured(
		RectangleFormData rect, 
		Texture* texture, 
		vec3 origin) abstract;
	virtual void drawRectangleTextured(
		float rect_width,
		float rect_height,
		Texture* texture,
		vec3 origin) abstract;
protected:
	GLFWwindow *m_window;
};

class IRendererWorld {
public:
	virtual void drawWorld() abstract;
	virtual float getCameraHeight() abstract;
};

class Renderer : public IRenderer {
public:
	virtual void init( GLFWwindow *window ) override;
	virtual void destroy( ) override;

	void setupVAO( );

	virtual void drawFrame( ) override;
	virtual void prepareFrame() override;
	virtual void resizeFrameBuffer( GLFWwindow *window, int width, int height );
	virtual void drawRectangleTextured(
		RectangleFormData rect,
		Texture* texture,
		vec3 origin) override;
	virtual void drawRectangleTextured(
		float rect_width,
		float rect_height,
		Texture* texture,
		vec3 origin) override;

	void createTextures( );

	VertexBaseEntity m_rectangle_entity{};
protected:
	void loadTexture( const fnv1a::value_type entry, const char *path );
public:
};

inline const auto g_renderer 
	= std::make_shared< Renderer >( );