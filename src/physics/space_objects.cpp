#include "physics.hpp"
#include "factory/definitions.hpp"
#include "input/input.hpp"

using namespace std;
using namespace math;


ObjectForm::ObjectForm(ObjectForm& form)
{
	m_type = form.m_type;
	switch (m_type)
	{
	case m_form_circle:
		m_data.m_circle = new CircleFormData();
		m_data.m_circle->m_radius = form.m_data.m_circle->m_radius;
		break;
	case form_rectangle:
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
	m_type = ObjectFormType::m_form_circle;
	m_data.m_circle = new CircleFormData();
	m_data.m_circle->m_radius = radius;
}
ObjectForm::ObjectForm(float width, float height)
{
	m_type = ObjectFormType::form_rectangle;
	m_data.m_rectangle = new RectangleFormData();
	m_data.m_rectangle->m_width = width;
	m_data.m_rectangle->m_height = height;
}
ObjectForm::ObjectForm()
{
	m_data.m_circle = nullptr;
	m_type = ObjectFormType::m_form_circle;
}

void ObjectForm::setData(float radius)
{
	if (m_type == form_rectangle)
	{
		releaseData();
		m_type = ObjectFormType::m_form_circle;
		m_data.m_circle = new CircleFormData();
	}
	m_data.m_circle->m_radius = radius;
}

void ObjectForm::setData(float width, float height)
{
	if (m_type == m_form_circle)
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
	case m_form_circle:
		if (m_data.m_circle != nullptr)
			delete m_data.m_circle;
		break;
	case form_rectangle:
		if (m_data.m_rectangle != nullptr)
			delete m_data.m_rectangle;
		break;
	default:
		break;
	}
}
ObjectForm::~ObjectForm()
{
	releaseData();
}

// Создаёт локальный объект с указанными параметрами
SpaceObject::SpaceObject(const District* current_district, ObjectForm&& form, Vector&& position,
	float scale, float rotation, float rotation_speed, float max_rotation_speed)
	: m_current_district(current_district), m_position(position), m_form(form), m_scale(scale),
	m_rotation(rotation), m_rotation_speed(rotation_speed), m_max_rotation_speed(max_rotation_speed)
{
		
}
// Создаёт локальный объект, загружая его из файла
SpaceObject::SpaceObject(const District* currentDistrict, FILE* f)
	: m_current_district(currentDistrict)
{
	load(f);
}
// Копирующий конструктор
SpaceObject::SpaceObject(SpaceObject& lo)
	: m_current_district(lo.m_current_district), m_position(lo.m_position), m_scale(lo.m_scale), m_form(lo.m_form),
	m_rotation(lo.m_rotation), m_rotation_speed(lo.m_rotation_speed), m_max_rotation_speed(lo.m_max_rotation_speed)
{
		
}

void SpaceObject::rotate(float da)
{
	m_rotation += da;
	if (m_rotation > g_pi2)
		m_rotation -= trunc(m_rotation / g_pi2) * g_pi2;
	else if (m_rotation < 0)
		m_rotation -= (trunc(m_rotation / g_pi2) - 1) * g_pi2;
}
void SpaceObject::moveTo(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
}
void SpaceObject::moveTo(float x, float y, float level)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = level;
}
void SpaceObject::moveTo(const District* district, float x, float y, float level)
{
	this->m_current_district = district;
}
void SpaceObject::load(FILE* f)
{
	
}

void SpaceObject::save(FILE* f)
{
	
}

MoveableObject::MoveableObject(const District* district, ObjectForm&& form, 
	Vector&& position, float scale, float rotation, float rotation_speed, 
	float max_rotation_speed,	glm::vec3&& speed_direction, float current_speed, 
	float max_speed, float acceleration)
	: SpaceObject(district, std::move(form), std::move(position), 
		scale, rotation, rotation_speed, max_rotation_speed), m_speed_direction(speed_direction),
	m_current_speed(current_speed), m_max_speed (max_speed), m_acceleration(acceleration)
{

}
void MoveableObject::move(float dt)
{
	m_position += dt * m_current_speed;
}

void MoveableObject::load(const FILE* f)
{

}

void MoveableObject::save(const FILE* f)
{

}
