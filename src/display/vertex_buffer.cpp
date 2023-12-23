#include "vertex_buffer.hpp"

void VertexBuffer::push( VertexCommand vert ) {
	m_commands.push_back( vert );
}

void VertexBuffer::draw( ) {
	for ( auto vertex_entry : m_commands ) {
		if ( vertex_entry.m_is_texture ) {
			Texture texture = vertex_entry.m_texture;

			glBindTexture( GL_TEXTURE_2D, texture.getTextureID( ) );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, texture.getDimensions( ).x, texture.getDimensions( ).y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.getPixels( ) );
			glEnable( GL_TEXTURE_2D );
		}
		
		glBegin( vertex_entry.m_render_type );

		for ( auto vertex : vertex_entry.m_vertices ) {
			glColor4f( vertex.m_color.x, vertex.m_color.y, vertex.m_color.z, vertex.m_color.w );
			if ( vertex_entry.m_is_texture ) {
				glTexCoord2f( vertex.m_coord.x, vertex.m_coord.y );
			}
			glVertex2f( vertex.x, vertex.y );
		}

		if ( vertex_entry.m_is_texture )
			glDisable( GL_TEXTURE_2D );

		glEnd( );
	}
}

void VertexBuffer::addRect( glm::vec2 mins, glm::vec2 maxs, glm::vec4 color, const fnv_edited::value_type entry ) {
	VertexCommand cmd; // create command

	cmd.m_is_texture = entry;
	if ( cmd.m_is_texture )
		cmd.m_texture = *g_textures->get( entry );

	cmd.m_render_type = GL_QUADS;
	cmd.m_vertices = {
		{ mins.x, mins.y, color, { 0.0, 0.0 } },
		{ mins.x, maxs.y, color, { 0.0, 1.0 } },
		{ maxs.x, maxs.y, color, { 1.0, 1.0 } },
		{ maxs.x, mins.y, color, { 1.0, 0.0 } },
	};

	push( cmd );
}

void VertexBuffer::clear( ) { m_commands.clear( ); }