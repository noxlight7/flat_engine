#pragma once

#include "../entry.hpp"

#include "vertex_buffer.hpp"

class Texture;
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
	
	void drawFrame( );
	void resizeFrameBuffer( GLFWwindow *window, int width, int height );

	void createTextures( );

protected:
	void loadTexture( const fnv_edited::value_type entry, const char *path );
protected:
	double m_aspect_ratio;
};

inline const auto g_renderer 
	= std::make_shared< Renderer >( );