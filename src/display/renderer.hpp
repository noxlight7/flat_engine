#pragma once

#include "../entry.hpp"

#include "shaders/shader.hpp"

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
};

static const auto g_renderer 
	= std::make_shared< Renderer >( );