#include "game_camera.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Camera::~Camera( )
	{}

RectangleGlobalArea Camera::getVisibleRect(int screen_width, int screen_height) {
	float dy = getHeight() * 2;
	float dx = dy * (float) screen_width / (float) screen_height;
	auto [x, y] = to_tuple(m_origin.getGlobalCoords());
	RectangleGlobalArea rect{
		.m_left = x - dx, .m_right = x + dx, 
		.m_bottom = y - dy, .m_top = y + dy 
	};
	return rect;
}
