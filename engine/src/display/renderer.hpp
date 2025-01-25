#pragma once

#include "../entry.hpp"

#include "shaders/shader.hpp"
#include "texture_manager.hpp"
#include "vertices.hpp"
#include "physics/base_physics.hpp"

class Texture;
class Sprite {
public:
	Sprite( const GLuint texture_id );
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
	virtual void init( GLFWwindow *window, std::vector<std::string> textures_names ) = 0;
	virtual void destroy( ) = 0;

	virtual void drawFrame( ) = 0;
	virtual void prepareFrame() = 0;
	virtual void resizeFrameBuffer( GLFWwindow *window, int width, int height ) = 0;

	virtual void drawRectangleTextured(
		RectangleFormData rect, 
		Texture* texture, 
		vec3 origin) = 0;
	virtual void drawRectangleTextured(
		float rect_width,
		float rect_height,
		Texture* texture,
		vec3 origin) = 0;
protected:
	GLFWwindow *m_window;
};

class IRendererWorld {
public:
	virtual void drawWorld() = 0;
	virtual float getCameraHeight() = 0;
};

class Renderer : public IRenderer {
public:
	virtual void init( GLFWwindow *window, std::vector<std::string> textures_names ) override;
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

	virtual void createTextures( );

	VertexBaseEntity m_rectangle_entity{};
protected:
	void loadTexture(std::string path);
public:
};

inline const auto g_renderer 
	= std::make_shared< Renderer >( );