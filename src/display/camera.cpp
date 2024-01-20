#include "camera.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::~Camera( )
	{}

void Camera::think( ) {
	// setup current view matrix.
	m_view_matrix = glm::lookAt(
		m_origin,										// camera position in world-space.
		m_origin - glm::vec3( 0.f, 0.f, m_origin.z ),   // camera eye vector at origin.
		glm::vec3( 0.0f, 1.0f, 0.0f )					// up direction
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