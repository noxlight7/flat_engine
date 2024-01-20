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

inline const auto g_textures 
	= std::make_shared< TextureManager >( );