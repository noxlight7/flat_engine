#pragma once

#include "../entry.hpp"

struct vert2 {
	ALWAYS_INLINE vert2( const glm::vec2 pos, const glm::vec2 coord, const glm::vec4 color )
		: m_pos{ pos }, m_coord{ coord }, m_color{ color } {}

	glm::vec2 m_pos;
	glm::vec2 m_coord;
	glm::vec4 m_color;
};

using Vertices2D = std::vector< vert2 >;