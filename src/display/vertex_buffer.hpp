#pragma once

#include "../entry.hpp"

class Texture {
public:
	Texture( ) = default;
	Texture( const char *path );
	~Texture( );

	ALWAYS_INLINE GLubyte *getPixels( ) const { return m_pixels; }

	ALWAYS_INLINE glm::vec2 getDimensions( ) const { return { m_width, m_height }; }

	ALWAYS_INLINE GLuint getTextureID( ) const { return m_texture_id; }
private:
	GLuint			m_texture_id;
	GLubyte			*m_pixels;
	int				m_width, m_height, m_comps;
};

struct vert2d {
	float		x, y;
	glm::vec4	m_color;
	glm::vec2	m_coord;
};

using vert_array = std::vector< vert2d >;

class VertexCommand {
public:
	vert_array				m_vertices;
	GLenum					m_render_type;
	bool					m_is_texture;
	Texture					m_texture;
};

class VertexBuffer {
public:
	VertexBuffer( ) = default;

	void push( VertexCommand vert );
	void draw( );
	void clear( );

	void addRect( glm::vec2 mins, glm::vec2 maxs, glm::vec4 color = { 1.0, 1.0, 1.0, 1.0 }, const fnv_edited::value_type entry = 0 );
private:
	std::vector< VertexCommand > m_commands;
};

class TextureManager {
public:
	TextureManager( ) = default;

	ALWAYS_INLINE void push( const fnv_edited::value_type entry, Texture texture ) {
		m_textures[ entry ] = texture;
	}

	ALWAYS_INLINE Texture *get( const fnv_edited::value_type entry ) { return &m_textures[ entry ]; }
private:
	std::unordered_map < fnv_edited::value_type, Texture > m_textures;
};

inline const auto g_vertex_buffer 
	= std::make_shared< VertexBuffer >( );

inline const auto g_textures 
	= std::make_shared< TextureManager >( );