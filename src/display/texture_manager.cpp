#include "texture_manager.hpp"
#include <stb/stb_image.h>

Texture::Texture( const char *path ) {
	m_pixels = stbi_load( path, &m_width, &m_height, &m_comps, NULL );

	glGenBuffers( 1, &m_texture_id );
	glBindTexture( GL_TEXTURE_2D, m_texture_id );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );

	float borderColor[ ] = { 0.7f, 0.5f, 0.3f, 1.0f };
	glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pixels );
	glBindTexture( GL_TEXTURE_2D, 0 );

}

Texture::~Texture( ) {
	//stbi_image_free( m_pixels );
}