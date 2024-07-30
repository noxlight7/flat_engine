#include "camera.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Camera::~Camera( )
	{}

void Camera::think( ) {
	// setup current view matrix.
	//m_view_matrix = lookAt(
	//	vec3(m_origin.m_coords, m_height), // camera position in world-space.
	//	vec3(m_origin.m_coords, 0), // camera eye vector at origin.
	//	glm::vec3( 0.0f, 1.0f, 0.0f ) // up direction
	//);
	m_view_matrix = lookAt(
		vec3(0.0f, 0.0f, m_height), // camera position in world-space.
		vec3(0.0f, 0.0f, 0.0f), // camera eye vector at origin.
		vec3(0.0f, 1.0f, 0.0f) // up direction
	);
}

glm::mat4 Camera::worldToScreen( const glm::vec3 origin, float model_scale ) const {
	// get model matrix.
	glm::mat4 model = glm::mat4( model_scale );

	// calculate world to screen matrix.
	return m_proj_matrix * m_view_matrix * glm::translate( model, origin );
}

void Camera::setProjection( float width, float height ) {
	m_proj_matrix = glm::perspective( 90.f, width / height, .1f, 1000.f );
}

RectangleGlobalArea Camera::getVisibleRect(int screen_width, int screen_height) {
	float dy = m_height * 2;
	float dx = dy * (float) screen_width / (float) screen_height;
	auto [x, y] = to_tuple(m_origin.getGlobalCoords());
	RectangleGlobalArea rect{
		.m_left = x - dx, .m_right = x + dx, 
		.m_bottom = y - dy, .m_top = y + dy 
	};
	return rect;
}
