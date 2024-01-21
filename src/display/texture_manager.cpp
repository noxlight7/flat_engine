#include "texture_manager.hpp"
#include <stb/stb_image.h>

Texture::Texture( const char *path ) {
	// flip y-axis for opengl renderer.
	stbi_set_flip_vertically_on_load( true );

	m_pixels = stbi_load( path, &m_width, &m_height, &m_comps, NULL );
	if ( m_pixels )
		setParameters( );
}

Texture::~Texture( ) {
	//stbi_image_free( m_pixels );
}

void Texture::setParameters( ) {
	glGenTextures( 1, &m_texture_id );
	//glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_texture_id );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pixels );
	glGenerateMipmap( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, 0 );
}