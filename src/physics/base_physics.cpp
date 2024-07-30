#include "base_physics.hpp"
#include "factory/definitions.hpp"
#include <glm/glm.hpp>

using namespace std;
using namespace math;
using namespace glm;

std::tuple<double, double> to_tuple(const glm::vec2& vec) {
	return std::make_tuple(vec.x, vec.y);
}

Position::Position(double x, double y)
{
	setFromGlobalCoords(x, y);
}

Position::Position(Vector coords, ivec2 index) : m_coords(coords), m_index(index) {
}

void Position::normalizeCoords() {
	int x_shift = (int)floorf(m_coords.x / g_cell_size);
	int y_shift = (int)floorf(m_coords.y / g_cell_size);
	m_coords.x -= x_shift * g_cell_size;
	m_coords.y -= y_shift * g_cell_size;
	m_index.x += x_shift;
	m_index.y += y_shift;
}

DVector Position::getGlobalCoords() const {
	return DVector(
		m_coords.x + m_index.x * g_cell_size,
		m_coords.y + m_index.y * g_cell_size
	);
}

void Position::setFromGlobalCoords(double x, double y) {
	m_index.x = (int)floor(x / g_cell_size);
	m_index.y = (int)floor(y / g_cell_size);

	m_coords.x = x - m_index.x * g_cell_size;
	m_coords.y = y - m_index.y * g_cell_size;

}

void Position::shiftToCoordsSystem(ivec2 cell_index) {
	m_coords.x += (m_index.x - cell_index.x) * g_cell_size;
	m_coords.y += (m_index.y - cell_index.y) * g_cell_size;
	m_index = cell_index;
}

void Position::shiftToCoordsSystem(Position pos) {
	shiftToCoordsSystem(pos.m_index);
}

vec2 Position::getShift(Position obj) {
	float x = (obj.m_index.x - m_index.x) * g_cell_size 
		+ obj.m_coords.x - m_coords.x;
	float y = (obj.m_index.y - m_index.y) * g_cell_size
		+ obj.m_coords.y - m_coords.y;
	return vec2(x, y);
}

ObjectForm::ObjectForm(ObjectForm& form)
{
	m_data.m_circle = nullptr;
	m_type = form.m_type;
	switch (m_type)
	{
	case FORM_CIRCLE:
		m_data.m_circle = new CircleFormData();
		m_data.m_circle->m_radius = form.m_data.m_circle->m_radius;
		break;
	case FORM_RECTANGLE:
		m_data.m_rectangle = new RectangleFormData();
		m_data.m_rectangle->m_width = form.m_data.m_rectangle->m_width;
		m_data.m_rectangle->m_height = form.m_data.m_rectangle->m_height;
		break;
	default:
		break;
	}
}

ObjectForm::ObjectForm(float radius)
{
	m_type = ObjectFormType::FORM_CIRCLE;
	m_data.m_circle = new CircleFormData();
	m_data.m_circle->m_radius = radius;
}
ObjectForm::ObjectForm(float width, float height)
{
	m_type = ObjectFormType::FORM_RECTANGLE;
	m_data.m_rectangle = new RectangleFormData();
	m_data.m_rectangle->m_width = width;
	m_data.m_rectangle->m_height = height;
}
ObjectForm::ObjectForm()
{
	m_data.m_circle = nullptr;
	m_type = ObjectFormType::FORM_CIRCLE;
}

void ObjectForm::setData(float radius)
{
	if (m_type == FORM_RECTANGLE)
	{
		releaseData();
		m_type = ObjectFormType::FORM_CIRCLE;
		m_data.m_circle = new CircleFormData();
	}
	m_data.m_circle->m_radius = radius;
}

void ObjectForm::setData(float width, float height)
{
	if (m_type == FORM_CIRCLE)
	{
		releaseData();
		m_data.m_rectangle = new RectangleFormData();
	}
	m_data.m_rectangle->m_width = width;
	m_data.m_rectangle->m_height = height;
}

void ObjectForm::releaseData()
{
	switch (m_type)
	{
	case FORM_CIRCLE:
		if (m_data.m_circle != nullptr)
			delete m_data.m_circle;
		break;
	case FORM_RECTANGLE:
		if (m_data.m_rectangle != nullptr)
			delete m_data.m_rectangle;
		break;
	default:
		break;
	}
}

vec2&& ObjectForm::getSize(float angle)
{
	vec2 size{};
	switch (m_type)
	{
	case ObjectFormType::FORM_CIRCLE:
		size.x = size.y = m_data.m_circle->m_radius * 2;
		break;
	case ObjectFormType::FORM_RECTANGLE:
		float x = m_data.m_rectangle->m_width;
		float y = m_data.m_rectangle->m_height;
		size.x = x * cos(angle) + y * sin(angle);
		size.y = y * cos(angle) + x * sin(angle);
		break;
	}
	return move(size);
}

float ObjectForm::getMaxSize()
{
	float res = 0;
	switch (m_type)
	{
	case ObjectFormType::FORM_CIRCLE:
		res = m_data.m_circle->m_radius * 2;
		break;
	case ObjectFormType::FORM_RECTANGLE:
		float x = m_data.m_rectangle->m_width;
		float y = m_data.m_rectangle->m_height;
		res = sqrt(x * x + y * y);
		break;
	}
	return res;
}

ObjectForm::~ObjectForm()
{
	releaseData();
}
