#pragma once

#include "../entry.hpp"

class Camera {
public:
	Camera( ) : m_origin{ 900.f, 900.f, 5.f } {};
	~Camera( );

	void think( );

	glm::mat4 worldToScreen( const glm::vec3 origin, float model_scale = 1.f ) const;

	ALWAYS_INLINE const auto getOrigin( ) const { return m_origin; }
	
	// @todo: remake it.
	ALWAYS_INLINE void setOrigin( const glm::vec3 origin ) { m_origin = origin; m_origin.z = 5.f; }

	void setProjection( float width, float height );
private:
	glm::vec3   m_origin{};
	glm::mat4   m_proj_matrix{};
	glm::mat4   m_view_matrix{};
};

inline const auto g_camera 
		= std::make_shared< Camera >( );