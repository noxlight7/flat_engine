#pragma once

#include "../entry.hpp"

#define GET_TEXTURE_SLOT( tex ) ( GL_TEXTURE0 + tex->getSlot( ) )

class Texture {
public:
	Texture( ) = default;
	explicit Texture( const std::string& path );
	~Texture( );

	[[nodiscard]] ALWAYS_INLINE GLubyte   *getPixels( ) const { return m_pixels; }

	[[nodiscard]] ALWAYS_INLINE glm::vec2  getDimensions( ) const { return { m_width, m_height }; }

	[[nodiscard]] ALWAYS_INLINE GLuint     getTextureID( ) const { return m_texture_id; }

	[[nodiscard]] ALWAYS_INLINE GLuint     getSlot( ) const { return m_texture_slot; }

	ALWAYS_INLINE void       setCurrentSlot( const GLuint new_slot ) { m_texture_slot = new_slot; }
private:
	void setParameters( );

	GLuint			m_texture_id{};
	GLuint			m_texture_slot{};
	GLubyte			*m_pixels = nullptr;
	int				m_width{}, m_height{}, m_comps{};
};

class TextureManager {
public:
	TextureManager( ) = default;

	GLuint push(std::string path) {
		if (m_texture_path_id.contains(path))
			return m_texture_path_id[path];

		Texture texture(path);

		// force texture slot.
		texture.setCurrentSlot( m_next_free_slot );

		// add texture to buffer.
		const GLuint texture_id = texture.getTextureID( );
		m_textures[ texture_id ] = texture;

		m_texture_path_id[std::move(path)] = texture_id;

		// increment next slot.
		m_next_free_slot++;

		return texture_id;
	}

	[[nodiscard]] ALWAYS_INLINE Texture *get( const GLuint entry ) { return &m_textures[ entry ]; }
	[[nodiscard]] ALWAYS_INLINE Texture *get( const std::string& path) {return &m_textures[m_texture_path_id[path]];}
	[[nodiscard]] ALWAYS_INLINE GLuint   getTextureID( const std::string& path ) { return m_texture_path_id[path]; }

	using TextureBuffer = std::unordered_map < GLuint, Texture >;
	using TexturePathId = std::unordered_map < std::string, GLuint >;
private:
	TextureBuffer   m_textures{};
	TexturePathId   m_texture_path_id{};
	GLuint			m_next_free_slot{};
};

inline const auto g_textures 
	= std::make_shared< TextureManager >( );
