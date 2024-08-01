#pragma once

#include "../entry.hpp"
#include "physics/base_physics.hpp"

class Camera {
public:
	Camera() : m_origin(0, 0), m_height(0) {}
	Camera(glm::vec3 origin) : m_origin(origin.x, origin.y), m_height(origin.z) {};
	~Camera( );

	void think( );

	glm::mat4 worldToScreen( const glm::vec3 origin, float model_scale = 1.f ) const;

	ALWAYS_INLINE glm::vec3 getGlobalOrigin( ) const { 
		return glm::vec3(m_origin.getGlobalCoords(), m_height); 
	}
	
	// @todo: remake it.
	ALWAYS_INLINE void setGlobalOrigin( const glm::vec3 origin ) { 
		m_origin.setFromGlobalCoords(origin.x, origin.y); 
		m_height = 5.f; 
	}
	ALWAYS_INLINE void setHeight(float height) { m_height = height; }

	ALWAYS_INLINE Position getOrigin() const { return m_origin; };
	ALWAYS_INLINE void setOrigin(Position pos) { m_origin = pos; }
	ALWAYS_INLINE float getHeight() const { return m_height; };

	void setProjection( float width, float height );
	RectangleGlobalArea getVisibleRect(int screen_width, int screen_height);
private:
	Position    m_origin;
	float		m_height{};
	glm::mat4   m_proj_matrix{};
	glm::mat4   m_view_matrix{};
};

inline const auto g_camera = std::make_shared< Camera >(glm::vec3(900.f, 900.f, 5.f));
