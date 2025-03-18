#pragma once

#include "../entry.hpp"
#include "physics/base_physics.hpp"
#include <camera.hpp>

class Camera : public ICamera{
public:
	Camera() : m_origin(0, 0) {}
	explicit Camera(glm::vec3 origin) : ICamera(origin.z), m_origin(origin.x, origin.y) {};
	Camera(Camera&& other) noexcept
	: ICamera(std::move(other)),                  // перемещаем базовую часть
	  m_origin(other.m_origin)        // перемещаем позицию
	  // m_proj_matrix(other.m_proj_matrix),
	  // m_view_matrix(other.m_view_matrix)
	{
		// Здесь можно добавить дополнительную логику, если необходимо.
	}
	~Camera() final;

	void think();

	glm::mat4 worldToScreen( const glm::vec3 origin, float model_scale = 1.f ) const;

	[[nodiscard]] ALWAYS_INLINE glm::vec3 getGlobalOrigin( ) const {
		return {vec3(m_origin.getGlobalCoords(), getHeight())};
	}
	
	// @todo: remake it.
	ALWAYS_INLINE void setGlobalOrigin( const glm::vec3 origin ) { 
		m_origin.setFromGlobalCoords(origin.x, origin.y); 
		setHeight(origin.z);
	}
	[[nodiscard]] ALWAYS_INLINE Position getOrigin() const { return m_origin; };
	ALWAYS_INLINE void setOrigin(Position pos) { m_origin = pos; }

	RectangleGlobalArea getVisibleRect(int screen_width, int screen_height);
private:
	Position    m_origin;
	// glm::mat4   m_proj_matrix{};
	// glm::mat4   m_view_matrix{};
};

// inline const auto g_camera = std::make_shared< Camera >(glm::vec3(900.f, 900.f, 5.f));
