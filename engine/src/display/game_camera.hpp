#pragma once

#include "../entry.hpp"
#include "physics/base_physics.hpp"
#include <camera.hpp>

class Camera : public ICamera{
public:
	Camera() : m_origin(0, 0) {}
	Camera(const Camera& camera) : m_origin(camera.m_origin) {}
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

	[[nodiscard]] glm::mat4 worldToScreen( const glm::vec3 origin, float model_scale = 1.f ) const;

	[[nodiscard]] ALWAYS_INLINE glm::dvec3 getGlobalOrigin( ) const {
		std::lock_guard lock(m_mutex);
		return {glm::vec3(m_origin.getGlobalCoords(), getHeight())};
	}
	
	// @todo: remake it.
	ALWAYS_INLINE void setGlobalOrigin( const glm::vec3 origin ) {
		std::lock_guard lock(m_mutex);
		m_origin.setFromGlobalCoords(origin.x, origin.y); 
		setHeight(origin.z);
	}
	[[nodiscard]] ALWAYS_INLINE Position getOrigin() const {
		std::lock_guard lock(m_mutex);
		return m_origin;
	};
	ALWAYS_INLINE void setOrigin(Position pos) {
		std::lock_guard lock(m_mutex);
		m_origin = pos;
	}

	RectangleArea getVisibleRect(int screen_width, int screen_height);
private:
	Position    m_origin;
	mutable std::mutex  m_mutex;
	// glm::mat4   m_proj_matrix{};
	// glm::mat4   m_view_matrix{};
};

// inline const auto g_camera = std::make_shared< Camera >(glm::vec3(900.f, 900.f, 5.f));
