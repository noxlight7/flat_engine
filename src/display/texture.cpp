#include "vertex_buffer.hpp"
#include <stb/stb_image.h>

Texture::Texture( const char *path ) {
	m_pixels = stbi_load( path, &m_width, &m_height, &m_comps, NULL );

	glGenBuffers( 1, &m_texture_id );
}

Texture::~Texture( ) {
	//stbi_image_free( m_pixels );
}