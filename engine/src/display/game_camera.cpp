#include "game_camera.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Camera::~Camera( )
	{}

RectangleArea Camera::getVisibleRect(int screen_width, int screen_height) {
	std::lock_guard lock(m_mutex);
	float dy = getHeight() * 2;
	float dx = dy * static_cast<float>(screen_width) / static_cast<float>(screen_height);
	auto [x, y] = to_tuple(m_origin.getGlobalCoords());
	RectangleArea rect{x - dx, x + dx, y - dy, y + dy};
	return rect;
}
