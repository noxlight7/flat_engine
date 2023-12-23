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

// Создаёт локальный объект с указанными параметрами
SpaceObject::SpaceObject(
	ObjectForm&& form,
	float scale, 
	float rotation, 
	float rotation_speed, 
	float max_rotation_speed)
	: m_current_district(nullptr), m_position(), m_form(form), 
	m_scale(scale),	m_rotation(rotation), 
	m_rotation_speed(rotation_speed), 
	m_max_rotation_speed(max_rotation_speed)
{
}
// Создаёт локальный объект, загружая его из файла
SpaceObject::SpaceObject(FILE* f)
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

void SpaceObject::initInNewDistrictNet()
{
	// Сюда добавить код создания матрицы информации о списках
	m_matrix_info = new ListElementMatrix<SpaceObject>(10, 10);
	m_matrix_info->shiftLeft();
}

void SpaceObject::moveTo(const District* district, float x, float y)
{
	if (m_current_district == nullptr ||
		m_current_district->m_net != district->m_net) {
		m_current_district = district;
		initInNewDistrictNet();
	}
	else
		m_current_district = district;
	
	moveTo(x, y);
	insertToDistrictList();
}

void SpaceObject::load(FILE* f)
{
	
}

void SpaceObject::save(FILE* f)
{
	
}

void SpaceObject::insertToDistrictList()
{
	m_district_info.insert(
		const_cast<list<SpaceObject*>*>
		(&m_current_district->m_space_objects), this);
}

void SpaceObject::removeFromDistrictList() {
	m_district_info.remove();
}

template <typename Type>
ListElementMatrix<Type>::ListElementMatrix(int row_amount, int column_amount)
	: m_row_amount(row_amount), m_column_amount(column_amount)
{
	m_buffer = new ListPElementInfo<Type>**[row_amount];

	for (int i = 0; i < row_amount; i++)
	{
		m_buffer[i] = new ListPElementInfo<Type>*[column_amount];

		for (int j = 0; j < column_amount; j++)
			m_buffer[i][j] = new ListPElementInfo<Type>();
	}
}

template <typename Type>
ListElementMatrix<Type>::~ListElementMatrix()
{
	for (int i = 0; i < m_row_amount; i++)
	{
		for (int j = 0; j < m_column_amount; j++)
		{
			m_buffer[i][j]->remove();
			delete[] m_buffer[i][j];
		}

		delete[] m_buffer[i];
	}

	delete[] m_buffer;
}

template <typename Type>
void ListElementMatrix<Type>::shiftLeft()
{
	int column = m_column_amount - 1;
	for (int i = 0; i < m_row_amount; i++)
		m_buffer[i][column]->remove();

	// Дописать функцию
}

template <typename Type>
void ListElementMatrix<Type>::shiftRight()
{

}

template <typename Type>
void ListElementMatrix<Type>::shiftTop()
{

}

template <typename Type>
void ListElementMatrix<Type>::shiftBottom()
{

}

template <typename Type>
ListPElementInfo<Type>*
ListElementMatrix<Type>::getInfo(int row, int column)
{

}

template <typename Type>
ListPElementInfo<Type>::ListPElementInfo() 
	: m_plist(nullptr)
{
}

template <typename Type>
ListPElementInfo<Type>::~ListPElementInfo()
{
	remove();
}

template <typename Type>
void ListPElementInfo<Type>::insert(list<Type*>* plist, Type* pval)
{
	if (m_plist != nullptr)
		remove();

	m_plist = plist;
	m_iterator = plist->insert(plist->end(), pval);
}

template <typename Type>
void ListPElementInfo<Type>::remove()
{
	if (m_plist != nullptr)
	{
		m_plist->erase(m_iterator);
		m_plist = nullptr;
	}
}

template <typename Type>
Type* ListPElementInfo<Type>::getElement()
{
	if (m_plist == nullptr)
		return nullptr;

	return *m_iterator;
}

MoveableObject::MoveableObject(
	ObjectForm&& form, 
	float scale, 
	float rotation, 
	float rotation_speed, 
	float max_rotation_speed, 
	glm::vec3&& speed_direction, 
	float max_speed, float acceleration)
	: SpaceObject(std::move(form), scale, rotation, 
		rotation_speed, max_rotation_speed), 
	m_speed_direction(speed_direction),
	m_current_speed (0), 
	m_max_speed (max_speed), 
	m_acceleration(acceleration)
{

}
void MoveableObject::move(float dt)
{
	m_position += dt * m_current_speed * m_speed_direction;
}

void MoveableObject::load(const FILE* f)
{

}

void MoveableObject::save(const FILE* f)
{

}

void MoveableObject::setSpeedDirection(Vector& speed_direction)
{
	if (speed_direction.x == 0 && speed_direction.y == 0)
		return;

	m_speed_direction = speed_direction;

	glm::normalize(m_speed_direction);
}

void MoveableObject::setCurrentSpeed(float current_speed)
{
	if (m_current_speed > m_max_speed)
		m_current_speed = m_max_speed;

	m_current_speed = current_speed;
}

glm::vec2 SpaceObject::getRenderOrigin( ) {
	return glm::vec2( m_position.x, -m_position.y );
}

Vector SpaceObject::getPosition()
{
	return m_position;
}

void MoveableObject::insertToDistrictList()
{
	SpaceObject::insertToDistrictList();

	m_district_moveable_info.insert(
		const_cast<list<MoveableObject*>*>
		(&m_current_district->m_moveable_objeсts), this);
}

void MoveableObject::removeFromDistrictList()
{
	SpaceObject::removeFromDistrictList();

	m_district_moveable_info.remove();
}