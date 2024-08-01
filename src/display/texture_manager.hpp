#pragma once

#include "../entry.hpp"

#define GET_TEXTURE_SLOT( tex ) ( GL_TEXTURE0 + tex->getSlot( ) )

class Texture {
public:
	Texture( ) = default;
	Texture( const char *path );
	~Texture( );

	ALWAYS_INLINE GLubyte   *getPixels( ) const { return m_pixels; }

	ALWAYS_INLINE glm::vec2  getDimensions( ) const { return { m_width, m_height }; }

	ALWAYS_INLINE GLuint     getTextureID( ) const { return m_texture_id; }

	ALWAYS_INLINE GLuint     getSlot( ) const { return m_texture_slot; }

	ALWAYS_INLINE void       setCurrentSlot( const GLuint new_slot ) { m_texture_slot = new_slot; }
private:
	void setParameters( );

	GLuint			m_texture_id;
	GLuint			m_texture_slot;
	GLubyte			*m_pixels;
	int				m_width, m_height, m_comps;
};

class TextureManager {
public:
	TextureManager( ) = default;

	ALWAYS_INLINE void push( const fnv1a::value_type entry, Texture texture ) {
		// force texture slot.
		texture.setCurrentSlot( m_next_free_slot );

		// add texture to buffer.
		m_textures[ entry ] = texture;

		// increment next slot.
		++m_next_free_slot;
	}

	ALWAYS_INLINE Texture *get( const fnv1a::value_type entry ) { return &m_textures[ entry ]; }

	using TextureBuffer = std::unordered_map < fnv1a::value_type, Texture >;
private:
	TextureBuffer   m_textures{};
	GLuint			m_next_free_slot{};
};

inline const auto g_textures 
	= std::make_shared< TextureManager >( );
